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
#include <array>
#include <vector>
#include <stdexcept>
#include <cstdarg>

#include "../nga_types.hpp"
#include "../core/nga_reusable.hpp"
#include "../core/nga_crypto.hpp"
#include "../core/nga_curl_request.hpp"

#include "nga_kraken_api_error.hpp"

namespace nga {
namespace kraken {
    
    class NGA_CPP_CLASS_API APIBase : protected CURLRequest {
    private:
        typedef std::array<String, 5> DefaultHeaders;
        typedef std::array<const char *, 5> PublicHeaders;
        typedef std::array<const char *, 7> PrivateHeaders;
        
        String _aKey;
        DataVector _pKey;
        DefaultHeaders _defaultHeaders;
        
        PublicHeaders publicHeaders() const;
        PrivateHeaders privateHeaders() const;
        
        static uint64_t nonceValue();
        static DefaultHeaders generateHeaders();
        
    protected:
        NGA_REQUIRES_LAST_NULL_ARG
        std::shared_ptr<DataVector> requestPublic(const char * NGA_NONNULL method,
                                                  std::shared_ptr<DataVector> && reusable = nullptr,
                                                  const char * NGA_NULLABLE firstQueryArg = nullptr, ...);
        
        NGA_REQUIRES_LAST_NULL_ARG
        std::shared_ptr<DataVector> requestPrivate(const char * NGA_NONNULL method,
                                                   std::shared_ptr<DataVector> && reusable = nullptr,
                                                   const char * NGA_NULLABLE firstPostArg = nullptr, ...);
    public:
        APIBase(String && apiKey, String && privateKey);
        APIBase();
        ~APIBase() noexcept = default;
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_API_BASE_HPP__
