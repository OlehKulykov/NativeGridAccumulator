/*
 * Copyright (C) 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_DEMAND_LOGGER_HPP__
#define __NGA_DEMAND_LOGGER_HPP__ 1

#include <atomic>
#include <filesystem>

#include "nga_logger.hpp"

namespace nga {
    
    class NGA_CPP_CLASS_API DemandLogger final : public Logger {
    private:
        std::atomic<int> _openCounter = 0;
        std::filesystem::path _path;
        
        virtual void open(const int fd) override final {
            Logger::open(fd);
        }
        
    public:
        virtual void open(const char * NGA_NONNULL path) override final;
        
        virtual void close() noexcept override final;
        
        DemandLogger & reopen();
        
        DemandLogger() noexcept = default;
        virtual ~DemandLogger() noexcept = default;
    };
        
} // namespace nga

#endif //!__NGA_DEMAND_LOGGER_HPP__
