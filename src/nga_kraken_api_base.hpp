/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_API_BASE_HPP__
#define __NGA_KRAKEN_API_BASE_HPP__ 1

#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdarg>

#include "r2d9/core/r2d9_data.hpp"
#include "r2d9/core/r2d9_reusable.hpp"
#include "r2d9/core/r2d9_crypto.hpp"

#include "nga_curl_request.hpp"

namespace nga {
namespace kraken {
    
    using r2d9::crypto::ZeroFillString;
    using r2d9::crypto::ZeroFillDataVector;
    
    class R2D9_CPP_CLASS_API APIBase : public CURLRequest {
    private:
        ZeroFillString _aKey;
        ZeroFillDataVector _pKey;
        
        static constexpr const size_t _defaultHeadersCount = 5;
        static const char * R2D9_NONNULL _defaultHeaders[_defaultHeadersCount];
        
    protected:
        R2D9_REQUIRES_LAST_NULL_ARG
        std::shared_ptr<ZeroFillDataVector> requestPublic(const char * R2D9_NONNULL method,
                                                          std::shared_ptr<ZeroFillDataVector> && reusable = nullptr,
                                                          const char * R2D9_NULLABLE firstQueryArg = nullptr, ...);
        
        R2D9_REQUIRES_LAST_NULL_ARG
        std::shared_ptr<ZeroFillDataVector> requestPrivate(const char * R2D9_NONNULL method,
                                                           std::shared_ptr<ZeroFillDataVector> && reusable = nullptr,
                                                           const char * R2D9_NULLABLE firstPostArg = nullptr, ...);
    public:
        APIBase(const char * R2D9_NONNULL apiKey, const char * R2D9_NONNULL privateKey);
        
        APIBase() noexcept = default;
        ~APIBase() noexcept = default;
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_API_BASE_HPP__
