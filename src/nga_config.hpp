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

namespace nga {
    
    class Config final {
    private:
        mutable std::mutex _mutex;
        mutable std::shared_ptr<kraken::Config> _krakenConfig;
        mutable std::shared_ptr<telegram::Config> _telegramConfig;
        std::filesystem::path _path;
        std::filesystem::path _logFilePath;
        
        void _clear() noexcept;
        
    public:
        std::filesystem::path path() const;
        
        std::shared_ptr<kraken::Config> krakenConfig() const;
        
        std::shared_ptr<telegram::Config> telegramConfig() const;
        
        std::filesystem::path logFilePath() const;
        
        void load(const char * NGA_NONNULL path);
        
        void clearKraken();
        
        void clearTelegram();
        
        void clear();
        
        Config() noexcept = default;
        ~Config() noexcept;
    };
    
} // namespace nga

#endif //!__NGA_CONFIG_HPP__
