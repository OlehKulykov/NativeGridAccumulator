/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_CONFIG_HPP__
#define __NGA_CONFIG_HPP__ 1

#include <memory>
#include <utility>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <string>
#include <filesystem>

#include "kraken/nga_kraken_config.hpp"
#include "telegram/nga_telegram_config.hpp"
#include "core/nga_trio.hpp"

namespace nga {
    
    class Config final {
    private:
        kraken::Config _krakenConfig;
        telegram::Config _telegramConfig;
        std::filesystem::path _path;
        std::filesystem::path _logFilePath;
        
    public:
        const std::filesystem::path & path() const noexcept { return _path; }
        const std::filesystem::path & logFilePath() const noexcept { return _logFilePath; }
        
        kraken::Config & krakenConfig() noexcept { return _krakenConfig; };
        telegram::Config & telegramConfig() noexcept { return _telegramConfig; };
        
        void load(const char * NGA_NONNULL path);
        
        Config() noexcept = default;
        ~Config() noexcept = default;
    };
    
} // namespace nga

#endif //!__NGA_CONFIG_HPP__
