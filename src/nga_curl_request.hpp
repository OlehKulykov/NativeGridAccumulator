/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_CURL_REQUEST_HPP__
#define __NGA_CURL_REQUEST_HPP__ 1

#include <memory>
#include <vector>
#include <stdexcept>

#include "r2d9/core/r2d9_crypto.hpp"

namespace nga {
    
    using r2d9::crypto::ZeroFillDataVector;
    
    class CURLRequest {
    public:
        static std::shared_ptr<ZeroFillDataVector> GET(const char * R2D9_NONNULL url,
                                                       const char * R2D9_NULLABLE headers[R2D9_NONNULL] = nullptr,
                                                       const size_t headersCount = 0,
                                                       std::shared_ptr<ZeroFillDataVector> && responceData = nullptr);
        
        static std::shared_ptr<ZeroFillDataVector> POST(const char * R2D9_NONNULL url,
                                                        const char * R2D9_NULLABLE headers[R2D9_NONNULL] = nullptr,
                                                        const size_t headersCount = 0,
                                                        const void * R2D9_NULLABLE postData = nullptr,
                                                        const size_t postDataSize = 0,
                                                        std::shared_ptr<ZeroFillDataVector> && responceData = nullptr);
        
        static void globalInit() noexcept;
        
        static void globalDeinit() noexcept;
    };
    
} // namespace nga

#endif //!__NGA_CURL_REQUEST_HPP__
