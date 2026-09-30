/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <memory>
#include <format>
#include <cstring>
#include <climits>
#include <ctime>

#include "nga_logger.hpp"
#include "nga_fixed_string_stream.hpp"
#include "nga_noexcept_unique_lock.hpp"

#include "nga_c_string.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <syslog.h>

namespace nga {
    
    const char * NGA_NONNULL const loggerTypeException = "EXC";
    const char * NGA_NONNULL const loggerTypeInfo      = "INF";
    const char * NGA_NONNULL const loggerTypeCritical  = "CRIT";
    const char * NGA_NONNULL const loggerTypeWarning   = "WARN";
    const char * NGA_NONNULL const loggerTypeDebug     = "DBG";
    
    void Logger::log(std::exception_ptr exception) const noexcept {
        try {
            if (exception) {
                std::rethrow_exception(exception);
            }
        } catch (const std::exception & exception) {
            log(loggerTypeException, exception.what());
        }
    }
    
    void Logger::log(const std::exception & exception) const noexcept {
        log(loggerTypeException, exception.what());
    }
    
    void Logger::log(const char * NGA_NULLABLE type, const char * NGA_NULLABLE format, ...) const noexcept {
        const auto now = std::chrono::system_clock::now();
        NoExceptUniqueLock<std::mutex> lock(_mutex);
        if (_fd >= 0) {
            FixedStringStream<63, false> stream; // 64 == bufferCapacity()
            stream << '[';
            stream.append(now, (TimePointFormatLocal | TimePointFormatSpaceSep)) << ']' << ' ';
            
            bool isCritical = false;
            if (type) {
                isCritical = (::strcmp(type, loggerTypeCritical) == 0);
                stream << ' ' << type << ' ' << ' ';
            }
            
            ssize_t wrRes = ::write(_fd, static_cast<const char *>(stream), stream.length());
            if (format) {
                va_list vaList;
                va_start(vaList, format);
                wrRes = ::vdprintf(_fd, format, vaList);
                va_end(vaList);
            }
            wrRes = ::write(_fd, "\n", 1);
            (void)wrRes;
            
            if (format && isCritical) {
                va_list vaList;
                va_start(vaList, format);
                Logger::systemLog(static_cast<int>(LOG_NDELAY | LOG_PERROR), static_cast<int>(LOG_ALERT | LOG_CRIT), format, vaList);
                va_end(vaList);
            }
        }
    }
    
    void Logger::open(const int fd) {
        const std::lock_guard<std::mutex> lock(_mutex);
        _close();
        if ((fd >= 0) && (::lseek(fd, 0, SEEK_END) >= 0)) {
            _fd = fd;
        } else {
            if (fd >= 0) {
                ::close(fd);
            }
            throw std::invalid_argument(std::format("Logger: invalid file descriptor: {}", fd));
        }
    }
    
    void Logger::open(const char * NGA_NONNULL path) {
        const std::lock_guard<std::mutex> lock(_mutex);
        _close();
        if (!path) {
            throw std::invalid_argument("Logger: path is null");
        }
        int fd;
        if ( (fd = ::open(path, static_cast<int>(O_WRONLY))) >= 0 ) {
            if (::lseek(fd, 0, SEEK_END) < 0) {
                ::close(fd);
                throw std::invalid_argument(std::format("Logger: invalid file descriptor: {}", fd));
            }
        } else if ( (fd = ::open(path, static_cast<int>(O_WRONLY | O_CREAT), static_cast<mode_t>(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))) < 0 ) {
            throw std::runtime_error(std::format("Logger: open file: {}, errno: {} ({})", path, errno, ::strerror(errno)));
        }
        _fd = fd;
    }
    
    void Logger::_close() noexcept {
        if (_fd >= 0) {
            ::close(_fd);
            _fd = -1;
        }
    }
    
    void Logger::close() noexcept {
        NoExceptUniqueLock<std::mutex> lock(_mutex);
        _close();
    }
    
    Logger::~Logger() noexcept {
        _close();
    }
    
    void Logger::systemLog(const int option,
                           const int priority,
                           const char * NGA_NONNULL format,
                           va_list vaList) noexcept {
        ::openlog("nga", option, LOG_DAEMON);
        ::vsyslog(priority, format, vaList);
        ::closelog();
    }
    
    void Logger::systemLog(const char * NGA_NULLABLE type, const char * NGA_NONNULL format, ...) noexcept {
        int option = LOG_NDELAY, priority = LOG_INFO;
        if (type) {
            if (::strcmp(type, loggerTypeCritical) == 0) {
                option |= LOG_PERROR;
                priority = LOG_ALERT | LOG_CRIT;
            } else if (::strcmp(type, loggerTypeException) == 0) {
                option |= LOG_PERROR;
                priority = LOG_ALERT | LOG_ERR;
            }
        }
        
        va_list vaList;
        va_start(vaList, format);
        Logger::systemLog(option, priority, format, vaList);
        va_end(vaList);
    }
    
} // namespace nga
