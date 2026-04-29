/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <algorithm>

#include "r2d9/core/r2d9_crypto.hpp"
#include "r2d9/core/r2d9_fixed_string_stream.hpp"
#include "r2d9/core/r2d9_scope_guard.hpp"
#include "r2d9/core/r2d9_c_string.h"

#include "nga_kraken_api_base.hpp"

namespace nga {
namespace kraken {

    using namespace r2d9;
    
    template<class T>
    inline static void APIBaseAppendURLQuery(T & url, va_list & list, const char * R2D9_NONNULL queryArg) {
        bool isKey = true;
        do {
            if (isKey) {
                url << '&';
            } else {
                url << '=';
            }
            url << queryArg;
            isKey = !isKey;
            queryArg = va_arg(list, const char *);
        } while (queryArg);
    }
    
    template<class T>
    inline static void APIBaseSetHost(T & url) {
        url << "https" << ':' << '/' << '/';
        url << "api" << '.';
        url << "kraken";
        url << '.' << "com";
    }
    
    const char * R2D9_NONNULL APIBase::_defaultHeaders[_defaultHeadersCount] = {
        "Accept: application/json",
        "Accept-Charset: utf-8",
        "Accept-Language: en-US",
        "Cache-Control: no-cache, no-store",
        "User-Agent: Kraken C++ API Client"
    };
    
    R2D9_REQUIRES_LAST_NULL_ARG
    std::shared_ptr<ZeroFillDataVector> APIBase::requestPublic(const char * R2D9_NONNULL method,
                                                               std::shared_ptr<ZeroFillDataVector> && reusable,
                                                               const char * R2D9_NULLABLE firstQueryArg, ...) {
        FixedStringStream<4095> url;
        ScopeGuard urlGuard([&] () {
            url.clear(true);
        });
        
        APIBaseSetHost(url);
        url << '/' << '0' << '/' << "public" << '/' << method;
        
        if (firstQueryArg) {
            url << '?';
            va_list list;
            va_start(list, firstQueryArg);
            ScopeGuard listGuard([&] () {
                va_end(list);
            });
            APIBaseAppendURLQuery(url, list, firstQueryArg);
        }
        
        return GET(url,
                   _defaultHeaders,
                   _defaultHeadersCount,
                   std::move(reusable));
    }
    
    // https://docs.kraken.com/api/docs/guides/spot-rest-auth
    R2D9_REQUIRES_LAST_NULL_ARG
    std::shared_ptr<ZeroFillDataVector> APIBase::requestPrivate(const char * R2D9_NONNULL method,
                                                                std::shared_ptr<ZeroFillDataVector> && reusable,
                                                                const char * R2D9_NULLABLE firstPostArg, ...) {
        if (_aKey.empty() || _pKey.empty()) {
            throw std::invalid_argument("Can't use private API without keys");
        }
        
        FixedStringStream<4095> payload;
        FixedStringStream<383> url;
        FixedStringStream<255> path;
        ScopeGuard streamsGuard([&] () {
            payload.clear(true);
            url.clear(true);
            path.clear(true);
        });
        
        APIBaseSetHost(url);
        path << '/' << '0' << '/' << "private" << '/' << method;
        url << path;
        
        FixedStringStream<65> nonce;
        nonce << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        payload << nonce << "nonce" << '=' << nonce;
        
        if (firstPostArg) {
            va_list list;
            va_start(list, firstPostArg);
            ScopeGuard listGuard([&] () {
                va_end(list);
            });
            APIBaseAppendURLQuery(payload, list, firstPostArg);
        }
        
        const auto payloadSha256 = crypto::sha256(payload, payload.length());
        if (path.availableLength() < payloadSha256.size()) {
            throw std::logic_error("Unsupported method");
        }
        
        ::memcpy(static_cast<char *>(path) + path.length(), payloadSha256.data(), payloadSha256.size());
        path.setLength(path.length() + payloadSha256.size());
        
        const auto pathHmacSha512 = crypto::hmacSha512(_pKey.data(), _pKey.size(), path, path.length());
        
        ZeroFillString headerAPIKey, headerAPISign;
        headerAPIKey.reserve(128);
        headerAPISign.reserve(128);
        headerAPIKey.append("API").append("-Key").append(": ").append(_aKey);
        headerAPISign.append("API").append("-Sign").append(": ").append(crypto::base64Encode(pathHmacSha512.data(), pathHmacSha512.size()));
        
        const char * headers[_defaultHeadersCount + 2];
        for (size_t i = 0; i < _defaultHeadersCount; i++) {
            headers[i] = _defaultHeaders[i];
        }
        headers[_defaultHeadersCount] = headerAPIKey.c_str();
        headers[_defaultHeadersCount + 1] = headerAPISign.c_str();
        
        return POST(url,
                    headers,
                    _defaultHeadersCount + 2,
                    static_cast<const char *>(payload) + nonce.length(),
                    payload.length() - nonce.length(),
                    std::move(reusable));
    }
    
    APIBase::APIBase(const char * R2D9_NONNULL apiKey, const char * R2D9_NONNULL privateKey) {
        if (!apiKey || !privateKey) {
            throw std::invalid_argument(emptyCString);
        }
        
        // exc -> no dtor, but local 'pKey/aKey'
        auto pKey = crypto::base64Decode(privateKey, ::strlen(privateKey));
        ZeroFillString aKey = apiKey;
        
        _pKey = std::move(pKey);
        _aKey = std::move(aKey);
    }
    
} // namespace kraken
} // namespace nga
