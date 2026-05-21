/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <cstdio>       // fopen
#include <cstdlib>      // strtoll
#include <cstdio>       // snprintf

#include "nga_scope_guard.hpp"

#include <unistd.h>     // getpid
#include <fcntl.h>
#include <sys/stat.h>

#include "nga_file_utils.hpp"

namespace nga {
namespace file {
    
    crypto::ZeroFillDataVector readContent(const char * NGA_NONNULL path, const size_t chunkSize) {
        const int fd = ::open(path, static_cast<int>(O_RDONLY));
        if (fd < 0) {
            throw std::runtime_error("Open for reading");
        }
        ScopeGuard guard([=] {
            ::close(fd);
        });
        
        size_t size = 0;
        {
            const off_t pos = ::lseek(fd, 0, SEEK_END);
            if (pos < 0) {
                throw std::runtime_error("Detect size");
            } else if (pos == 0) {
                return crypto::ZeroFillDataVector();
            }
            if (::lseek(fd, 0, SEEK_SET) != 0) {
                throw std::runtime_error("Start reading");
            }
            size = static_cast<size_t>(pos);
        }
        
        crypto::ZeroFillDataVector content;
        content.resize(size);
        
        size_t done = 0;
        uint8_t * buff = static_cast<uint8_t *>(content.data());
        while (done < size) {
            const ssize_t res = ::read(fd, buff, chunkSize);
            if ( (res < 0) || ((res == 0) && (done < size)) ) {
                throw std::runtime_error("Reading content");
            }
            buff += res;
            done += res;
        }
        
        if (done != size) {
            throw std::runtime_error("Content size mismatch");
        }
        
        return content;
    }
    
    pid_t readPID(const char * NGA_NONNULL path) {
        FILE * f = ::fopen(path, "rb");
        if (!f) {
            return 0;
        }
        
        char buff[32];
        const size_t r = ::fread(static_cast<char *>(buff), 1, 32, f);
        ::fclose(f);
        if (r == 0) {
            return 0;
        }
        
        if (r < 32) {
            buff[r] = 0;
            const auto pid = ::strtoll(static_cast<const char *>(buff), nullptr, 10);
            if (pid > 0) {
                return static_cast<pid_t>(pid);
            }
        }
        
        throw std::runtime_error("Reading PID file content");
    }
    
    void writePID(const char * NGA_NONNULL path) {
        const pid_t p = ::getpid();
        if (p <= 0) {
            throw std::runtime_error("Get PID");
        }
        
        char buff[32];
        const int len = ::snprintf(static_cast<char *>(buff), 32, "%" PRIu64 "\n", static_cast<uint64_t>(p));
        if (len <= 0) {
            throw std::runtime_error("Generate PID string");
        }
        
        FILE * f = ::fopen(path, "w+b");
        if (!f) {
            char reason[256];
            ::snprintf(reason, 256, "Open PID file for writing, errno: %i (%s)", errno, ::strerror(errno));
            throw std::runtime_error(reason);
        }
        
        bool isWriteError = false;
        if (::fwrite(static_cast<const char *>(buff), len, 1, f) != 1) {
            isWriteError = true;
        }
        ::fclose(f);
        
        if (isWriteError) {
            throw std::runtime_error("Write PID string to file");
        }
    }
    
    void stdioToDevNull(void) {
        if ((::close(STDIN_FILENO) != 0) || (::close(STDOUT_FILENO) != 0) || (::close(STDERR_FILENO) != 0)) {
            throw std::runtime_error("Close std file descriptors");
        }
        
        static const char * dnPath = "/dev/null";
        if ((::freopen(dnPath, "r", ::stdin) == nullptr) || (::freopen(dnPath, "w", ::stdout) == nullptr) || (::freopen(dnPath, "w", ::stderr) == nullptr)) {
            char reason[256];
            ::snprintf(reason, 256, "Reopen std file descriptors, errno: %i (%s)", errno, ::strerror(errno));
            throw std::runtime_error(reason);
        }
    }
    
} // namespace file
} // namespace nga
