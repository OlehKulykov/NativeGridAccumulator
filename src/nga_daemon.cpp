/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <memory>
#include <algorithm>
#include <utility>
#include <cstdlib>
#include <cstdio>
#include <cerrno>

#include "nga_config.hpp"
#include "kraken/nga_kraken_app.hpp"
#include "core/nga_file_utils.hpp"
#include "core/nga_c_string.h"
#include "core/nga_version.h"

#include <unistd.h>
#include <uv.h>

#if __has_include(<sys/mman.h>)
#  include <sys/mman.h>
#  define HAVE_SYS_MMAN_H 1
#endif

typedef std::pair<off_t, time_t> FSStat;

struct App final {
    std::unique_ptr<nga::kraken::App> krakenApp;
    std::shared_ptr<nga::Logger> logger;
    const char * configPath;
    uv_fs_event_t configEvent;
    FSStat configStat;
};

static void printHelp(void) noexcept {
    std::flush(std::cout) << nga_version_string() << std::endl;
    std::cout << "-h, --help   Display this help text and exit" << std::endl;
    std::cout << "-c           <configuration file path>" << std::endl;
}

static const char * findConfigPath(int argc, const char * argv[]) noexcept {
#if defined(__APPLE__) && defined(DEBUG)
    return "/tmp/config.json";
#endif
    const char * res = nullptr;
    for (int i = 0; i < argc; i++) {
        const char * arg = argv[i];
        if (!arg) continue;
        
        if ((::strcmp(arg, "--help") == 0) || (::strcmp(arg, "-h") == 0)) {
            printHelp();
            std::exit(EXIT_SUCCESS);
        } else if ((::strcmp(arg, "-c") == 0) && ((i + 1) < argc)) {
            res = argv[++i];
        }
    }
    if (!res) {
        printHelp();
        std::exit(EXIT_FAILURE);
    }
    return res;
}

static FSStat pathFSStat(const char * path) noexcept {
    struct stat st;
    if (::stat(path, &st) == 0) {
        return { std::max<off_t>(0, st.st_size), std::max<time_t>(0, st.st_mtime) };
    }
    return {0, 0};
}

static void onExit(uv_signal_t * handle, int signum) noexcept {
    auto * loop = ::uv_default_loop();
    auto & app = *static_cast<::App *>(loop->data);
    app.logger->log(nga::loggerTypeInfo, "Received exit signal %i. Exiting...", signum);
    app.krakenApp->onExit();
    ::uv_stop(loop);
}

static void onTick(uv_timer_t * handle) noexcept {
    auto & app = *static_cast<::App *>(handle->data);
    app.krakenApp->tick();
}

static void onConfigChanged(uv_fs_event_t * handle, const char * filename, int events, int) noexcept {
    auto & app = *static_cast<::App *>(handle->data);
    const auto stat = pathFSStat(app.configPath);
    const bool valid = (stat.first > 0) && (stat.second > 0);
    const bool changed = valid && ((stat.first != app.configStat.first) || (stat.second != app.configStat.second));
    if (changed) {
        app.configStat = stat;
    }
    
    if (events & UV_RENAME) {
        ::uv_fs_event_stop(handle);
        ::uv_fs_event_init(::uv_default_loop(), &app.configEvent);
        ::uv_fs_event_start(&app.configEvent, onConfigChanged, app.configPath, UV_FS_EVENT_RECURSIVE);
    }
    
    if (changed) {
        app.logger->log(nga::loggerTypeInfo, "Config changed, reloading");
        try {
            nga::Config config;
            config.load(app.configPath);
            
            app.krakenApp->onChanged(std::move(config.krakenConfig()));
        } catch (...) {
            app.logger->log(std::current_exception());
        }
    }
}

int main(int argc, const char * argv[]) {
#if !defined(__APPLE__) && defined(HAVE_SYS_MMAN_H)
    if (::mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
        nga::Logger::systemLog(nga::loggerTypeCritical, "Error lock all pages, errno: %i (%s)", errno, ::strerror(errno));
        return EXIT_FAILURE;
    }
#endif
    ::App app;
    app.configPath = findConfigPath(argc, argv);
    try {
        nga::Config config;
        config.load(app.configPath);
        
        auto logger = std::make_shared<nga::Logger>();
        logger->open(config.logFilePath().c_str());
        nga::file::stdioToDevNull();
        
        logger->log(nullptr, emptyCString);
        logger->log(nullptr, "Start[%" PRIi64 "]: %s, %s", static_cast<int64_t>(::getpid()), nga_version_string(), nga_build_string());
        app.logger = std::move(logger);
        
        auto krakenApp = std::make_unique<nga::kraken::App>();
        krakenApp->init(std::move(config.krakenConfig()), app.logger);
        app.krakenApp = std::move(krakenApp);
        
    } catch (const std::exception & exception) {
        if (app.logger) {
            app.logger->log(nga::loggerTypeCritical, "Init app, what: %s", (exception.what() ?: emptyCString));
        } else {
            nga::Logger::systemLog(nga::loggerTypeCritical, "Init app, what: %s", (exception.what() ?: emptyCString));
        }
        return EXIT_FAILURE;
    }
    
    auto * loop = ::uv_default_loop();
    uv_timer_t timer;
    loop->data = timer.data = &app;
    ::uv_timer_init(loop, &timer);
    ::uv_timer_start(&timer, onTick, 1000, 1000);
    
    const int16_t signums[5] = { SIGHUP, SIGINT, SIGQUIT, SIGABRT, SIGTERM };
    uv_signal_t signals[5];
    for (size_t i = 0; i < 5; i++) {
        auto * signal = static_cast<uv_signal_t *>(signals) + i;
        signal->data = &app;
        ::uv_signal_init(loop, signal);
        ::uv_signal_start_oneshot(signal, onExit, signums[i]);
    }
    
    app.configStat = pathFSStat(app.configPath);
    app.configEvent.data = &app;
    ::uv_fs_event_init(loop, &app.configEvent);
    ::uv_fs_event_start(&app.configEvent, onConfigChanged, app.configPath, UV_FS_EVENT_RECURSIVE);
    
    return ::uv_run(loop, UV_RUN_DEFAULT);
}
