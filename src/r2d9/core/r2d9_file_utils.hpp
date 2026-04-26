/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __R2D9_FILE_UTILS_HPP__
#define __R2D9_FILE_UTILS_HPP__ 1

#include <cstddef>

#include "r2d9_raw_heap_memory.hpp"

namespace r2d9 {
namespace file {
    
    R2D9_CPP_API_PRIVATE(RawHeapMemory) readContent(const char * R2D9_NONNULL path, const size_t chunkSize = 262144); // 256K
    R2D9_CPP_API_PRIVATE(pid_t) readPID(const char * R2D9_NONNULL path);
    R2D9_CPP_API_PRIVATE(void) writePID(const char * R2D9_NONNULL path);
    R2D9_CPP_API_PRIVATE(void) stdioToDevNull(void);
    
} // namespace file
} // namespace r2d9

#endif //!__R2D9_FILE_UTILS_HPP__
