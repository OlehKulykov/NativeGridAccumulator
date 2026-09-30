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

#include "nga_crypto.hpp"

namespace nga {
    
    class CURLRequest {
    public:
        static std::shared_ptr<crypto::ZeroFillDataVector> GET(const char * NGA_NONNULL url,
                                                               const char * NGA_NULLABLE headers[NGA_NONNULL] = nullptr,
                                                               const size_t headersCount = 0,
                                                               std::shared_ptr<crypto::ZeroFillDataVector> && responceData = nullptr);
        
        static std::shared_ptr<crypto::ZeroFillDataVector> POST(const char * NGA_NONNULL url,
                                                                const char * NGA_NULLABLE headers[NGA_NONNULL] = nullptr,
                                                                const size_t headersCount = 0,
                                                                const void * NGA_NULLABLE postData = nullptr,
                                                                const size_t postDataSize = 0,
                                                                std::shared_ptr<crypto::ZeroFillDataVector> && responceData = nullptr);
        
        static crypto::ZeroFillString generateUserAgent();
        
        static void globalInit() noexcept;
        
        static void globalDeinit() noexcept;
    };
    
} // namespace nga

#endif //!__NGA_CURL_REQUEST_HPP__
