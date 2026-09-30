/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_FILE_UTILS_HPP__
#define __NGA_FILE_UTILS_HPP__ 1

#include <cstddef>

#include "nga_crypto.hpp"

namespace nga {
namespace file {
    
    NGA_CPP_API_PRIVATE(crypto::ZeroFillDataVector) readContent(const char * NGA_NONNULL path, const size_t chunkSize = 262144); // 256K
    NGA_CPP_API_PRIVATE(pid_t) readPID(const char * NGA_NONNULL path);
    NGA_CPP_API_PRIVATE(void) writePID(const char * NGA_NONNULL path);
    NGA_CPP_API_PRIVATE(void) stdioToDevNull(void);
    
} // namespace file
} // namespace nga

#endif //!__NGA_FILE_UTILS_HPP__
