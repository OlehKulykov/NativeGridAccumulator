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
    
    class Logger final {
    private:
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
        
        void open(const int fd);
        void open(const char * NGA_NONNULL path);
        
        void close() noexcept;
        
        Logger() noexcept = default;
        ~Logger() noexcept;
        
        static void systemLog(const char * NGA_NULLABLE type, const char * NGA_NONNULL format, ...) noexcept;
    };

} // namespace nga

#endif //!__NGA_LOGGER_HPP__
