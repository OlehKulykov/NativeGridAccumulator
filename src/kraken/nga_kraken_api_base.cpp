/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <stdexcept>
#include <format>
#include <algorithm>
#include <ctime>

#include "../core/nga_http.hpp"
#include "../core/nga_crypto.hpp"
#include "../core/nga_fixed_string_stream.hpp"
#include "../core/nga_scope_guard.hpp"
#include "../core/nga_c_string.h"

#include <sys/time.h>
#include <sys/utsname.h>

#include <curl/curl.h>

#include "nga_kraken_api_base.hpp"

namespace nga {
namespace kraken {
    
    template<class T>
    inline static void APIBaseAppendURLQuery(T & url, va_list & list, const char * NGA_NONNULL queryArg) {
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
        url << "http" << 's' << ':' << '/' << '/';
        url << "api" << '.';
        url << "kra" << "ken";
        url << '.' << "com";
    }
    
    NGA_REQUIRES_LAST_NULL_ARG
    std::shared_ptr<DataVector> APIBase::requestPublic(const char * NGA_NONNULL method,
                                                       std::shared_ptr<DataVector> && reusable,
                                                       const char * NGA_NULLABLE firstQueryArg, ...) {
        FixedStringStream<4095> url;
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
        
        auto headers = publicHeaders();
        return GET(url, headers.data(), headers.size(), std::move(reusable));
    }
    
    // https://docs.kraken.com/api/docs/guides/spot-rest-auth
    NGA_REQUIRES_LAST_NULL_ARG
    std::shared_ptr<DataVector> APIBase::requestPrivate(const char * NGA_NONNULL method,
                                                        std::shared_ptr<DataVector> && reusable,
                                                        const char * NGA_NULLABLE firstPostArg, ...) {
        if (_aKey.empty() || _pKey.empty()) {
            throw std::invalid_argument("API: can't use private API without keys");
        }
        
        FixedStringStream<4095> payload;
        FixedStringStream<383> url;
        FixedStringStream<255> path;
        
        APIBaseSetHost(url);
        path << '/' << '0' << '/' << "private" << '/' << method;
        url << path;
        
        FixedStringStream<65> nonce;
        nonce << nonceValue();
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
            throw std::logic_error("API: unsupported method");
        }
        
        ::memcpy(static_cast<char *>(path) + path.length(), payloadSha256.data(), payloadSha256.size());
        path.setLength(path.length() + payloadSha256.size());
        
        const auto pathHmacSha512 = crypto::hmacSha512(_pKey.data(), _pKey.size(), path, path.length());
        
        String headerAPIKey, headerAPISign;
        headerAPIKey.reserve(128);
        headerAPISign.reserve(128);
        headerAPIKey.append("API").append("-Key").append(": ").append(_aKey);
        headerAPISign.append("API").append("-Sign").append(": ").append(crypto::base64Encode(pathHmacSha512.data(), pathHmacSha512.size()));
        
        auto headers = privateHeaders();
        headers[headers.size() - 2] = headerAPIKey.c_str();
        headers[headers.size() - 1] = headerAPISign.c_str();
        
        return POST(url,
                    headers.data(),
                    headers.size(),
                    static_cast<const char *>(payload) + nonce.length(),
                    payload.length() - nonce.length(),
                    std::move(reusable));
    }
    
    APIBase::PublicHeaders APIBase::publicHeaders() const {
        PublicHeaders res;
        for (size_t i = 0; i < _defaultHeaders.size(); i++) {
            res[i] = _defaultHeaders[i].c_str();
        }
        return res;
    }
    
    APIBase::PrivateHeaders APIBase::privateHeaders() const {
        PrivateHeaders res{nullptr};
        for (size_t i = 0; i < _defaultHeaders.size(); i++) {
            res[i] = _defaultHeaders[i].c_str();
        }
        return res;
    }
    
    APIBase::APIBase(String && apiKey, String && privateKey) {
        auto headers = generateHeaders();
        auto pKey = std::move(privateKey);
        _pKey = crypto::base64Decode(pKey.c_str(), pKey.size());
        _aKey = std::move(apiKey);
        _defaultHeaders = std::move(headers);
    }
    
    APIBase::APIBase() {
        _defaultHeaders = generateHeaders();
    }
    
    // UTC microseconds
    // The value for the nonce payload body parameter is an always increasing,
    //   unsigned 64-bit integer for each request that is made with a particular API key.
    ///@link https://docs.kraken.com/api/docs
    uint64_t APIBase::nonceValue() {
        struct timespec tms;
        if (::clock_gettime(CLOCK_REALTIME, &tms) != 0) {
            throw std::runtime_error(std::format("API: error retrieve current time, errno: {} ({})", errno, ::strerror(errno)));
        }
        return (static_cast<uint64_t>(tms.tv_sec) * 1000000) + (tms.tv_nsec / 1000);
    }
    
    APIBase::DefaultHeaders APIBase::generateHeaders() {
        DefaultHeaders headers;
        for (size_t i = 0; i < headers.size(); i++) {
            StringStream stream;
            switch (i) {
                case 0:  stream << http::headerAccept         << ": " << "application/json"; break;
                case 1:  stream << http::headerAcceptLanguage << ": " << "en-US"; break;
                case 2:  stream << http::headerAcceptCharset  << ": " << "utf-8"; break;
                case 3:  stream << http::headerCacheControl   << ": " << "no-cache, no-store"; break;
                default: stream << http::headerUserAgent      << ": " << generateUserAgent(); break;
            }
            headers[i] = stream.str();
        }
        return headers;
    }
    
} // namespace kraken
} // namespace nga
