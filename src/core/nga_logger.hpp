/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_LOGGER_HPP__
#define __NGA_LOGGER_HPP__ 1

#include <mutex>
#include <filesystem>
#include <atomic>
#include <stdexcept>
#include <cstdio>
#include <cstdarg>

#include "../nga.hpp"

namespace nga {

    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) loggerTypeException;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) loggerTypeInfo;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) loggerTypeCritical;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) loggerTypeWarning;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) loggerTypeDebug;
    
    class Logger {
    protected:
        mutable std::mutex _mutex;
        int _fd = -1;
        
        void _close() noexcept;
        
        static void systemLog(const int option,
                              const int priority,
                              const char * NGA_NONNULL format,
                              va_list vaList) noexcept;
        
    public:
        void log(const char * NGA_NULLABLE type, const char * NGA_NULLABLE format, ...) const noexcept;
        void log(const std::exception & exception) const noexcept;
        void log(std::exception_ptr exception) const noexcept;
        
        virtual void open(const int fd);
        virtual void open(const char * NGA_NONNULL path);
        
        virtual void close() noexcept;
        
        Logger() noexcept = default;
        virtual ~Logger() noexcept;
        
        static void systemLog(const char * NGA_NULLABLE type, const char * NGA_NONNULL format, ...) noexcept;
    };

    class DemandLogger final : public Logger {
    private:
        std::atomic<int> _openCounter = 0;
        std::filesystem::path _path;
        
        virtual void open(const int fd) override final {
            Logger::open(fd);
        }
        
    public:
        virtual void open(const char * NGA_NONNULL path) override final;
        
        virtual void close() noexcept override final;
        
        void reopen();
        
        DemandLogger() noexcept = default;
        virtual ~DemandLogger() noexcept = default;
    };
    
} // namespace nga

#endif //!__NGA_LOGGER_HPP__
