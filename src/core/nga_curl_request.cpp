/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <stdexcept>
#include <format>
#include <atomic>
#include <algorithm>
#include <cstddef>
#include <cstring>

#include "nga_fixed_string_stream.hpp"
#include "nga_trio.hpp"
#include "nga_scope_guard.hpp"
#include "nga_c_string.h"

#include <curl/curl.h>

#if __has_include(<sys/utsname.h>)
#  include <sys/utsname.h>
#  define HAVE_SYS_UTSNAME_H 1
#endif

#include "nga_curl_request.hpp"

namespace nga {
    
    struct CURLDeleter final {
        void operator()(CURL * NGA_NULLABLE curl) const noexcept {
            if (curl) {
                ::curl_easy_cleanup(curl);
            }
        }
    };
    
    struct CURLSListDeleter final {
        void operator()(struct curl_slist * NGA_NULLABLE sList) const noexcept {
            if (sList) {
                ::curl_slist_free_all(sList);
            }
        }
    };
    
    typedef std::unique_ptr<CURL, CURLDeleter> CURLUPtr;
    typedef std::unique_ptr<struct curl_slist, CURLSListDeleter> CURLSListUPtr;
    typedef std::pair<CURLUPtr, CURLSListUPtr> CURLBaseRequest;
    
    static size_t CURLRequestWriteDataCallback(char * NGA_NULLABLE contents, size_t size, size_t nmemb, void * NGA_NULLABLE userp) noexcept {
        size_t writeSize = size * nmemb;
        try {
            if (contents && writeSize) {
                auto * user = static_cast<crypto::ZeroFillDataVector *>(userp);
                user->insert(user->end(), reinterpret_cast<uint8_t *>(contents), reinterpret_cast<uint8_t *>(contents) + writeSize);
                ::memset(contents, 0, writeSize);
            }
        } catch (...) {
            if (contents && writeSize) {
                ::memset(contents, 0, writeSize);
            }
            writeSize = 0;
        }
        return writeSize;
    }
    
    static size_t CURLRequestReadDataCallback(char * NGA_NULLABLE contents, size_t size, size_t nmemb, void * NGA_NULLABLE userp) noexcept {
        auto * user = static_cast<TrioPOD<const char *, size_t, size_t> *>(userp);
        const size_t readSize = std::min(user->second - user->third, size * nmemb);
        if (contents && readSize) {
            ::memcpy(contents, user->first + user->third, readSize);
            user->third += readSize;
            return readSize;
        }
        return 0;
    }
    
    static CURLBaseRequest CURLRequestCreateBaseRequest(const char * NGA_NONNULL url,
                                                        const char * NGA_NULLABLE * NGA_NULLABLE headers,
                                                        const size_t headersCount) {
        ///@link https://curl.se/libcurl/c/httpcustomheader.html
        CURLSListUPtr headersSList;
        {
            struct curl_slist * prevList = nullptr;
            for (size_t i = 0; i < headersCount; i++) {
                const char * header = headers[i];
                if (!header) {
                    continue;
                }
                struct curl_slist * newList = ::curl_slist_append(prevList, header);
                if (newList) {
                    prevList = newList;
                } else {
                    if (prevList) {
                        ::curl_slist_free_all(prevList);
                    }
                    throw std::runtime_error("cURL: append headers");
                }
            }
            headersSList = CURLSListUPtr(prevList);
        }
        
        CURLUPtr curl(::curl_easy_init());
        if (!curl) {
            throw std::runtime_error("cURL: GET init");
        }
        
        CURLcode res;
        if (headersSList && ((res = curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headersSList.get())) != CURLE_OK) ) {
            throw std::runtime_error(std::format("cURL: set headers: {}", ::curl_easy_strerror(res)));
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_URL, url)) != CURLE_OK ) {
            throw std::runtime_error(std::format("cURL: set URL: {}", ::curl_easy_strerror(res)));
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 30)) != CURLE_OK ) {
            throw std::runtime_error(std::format("cURL: set timeout: {}", ::curl_easy_strerror(res)));
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 1L)) != CURLE_OK ) {
            throw std::runtime_error(std::format("cURL: set SSL verify peer: {}", ::curl_easy_strerror(res)));
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYHOST, 2L)) != CURLE_OK ) {
            throw std::runtime_error(std::format("cURL: set SSL verify host: {}", ::curl_easy_strerror(res)));
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, CURLRequestWriteDataCallback)) != CURLE_OK ) {
            throw std::runtime_error(std::format("cURL: set write: {}", ::curl_easy_strerror(res)));
        }
        
        return CURLBaseRequest(static_cast<CURLUPtr &&>(curl), static_cast<CURLSListUPtr &&>(headersSList));
    }
    
    std::shared_ptr<crypto::ZeroFillDataVector> CURLRequest::GET(const char * NGA_NONNULL url,
                                                                 const char * NGA_NULLABLE headers[NGA_NONNULL],
                                                                 const size_t headersCount,
                                                                 std::shared_ptr<crypto::ZeroFillDataVector> && responceData) {
        CURLRequest::globalInit();
        ScopeGuard lock([] {
            CURLRequest::globalDeinit();
        });
        
        auto base = CURLRequestCreateBaseRequest(url, headers, headersCount);
        
        auto movedData = static_cast<std::shared_ptr<crypto::ZeroFillDataVector> &&>(responceData);
        if (movedData) {
            movedData->clear();
        } else {
            movedData = std::make_shared<crypto::ZeroFillDataVector>();
        }
        
        CURLcode res;
        if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_WRITEDATA, movedData.get())) != CURLE_OK ) {
            throw std::runtime_error(std::format("cURL: write data: {}", ::curl_easy_strerror(res)));
        }
        
        if ( (res = ::curl_easy_perform(base.first.get())) != CURLE_OK ) {
            throw std::runtime_error(std::format("cURL: perform: {}", ::curl_easy_strerror(res)));
        }
        
        return movedData;
    }
    
    std::shared_ptr<crypto::ZeroFillDataVector> CURLRequest::POST(const char * NGA_NONNULL url,
                                                                  const char * NGA_NULLABLE headers[NGA_NONNULL],
                                                                  const size_t headersCount,
                                                                  const void * NGA_NULLABLE postData,
                                                                  const size_t postDataSize,
                                                                  std::shared_ptr<crypto::ZeroFillDataVector> && responceData) {
        CURLRequest::globalInit();
        ScopeGuard lock([] {
            CURLRequest::globalDeinit();
        });
        
        auto base = CURLRequestCreateBaseRequest(url, headers, headersCount);
        
        CURLcode res;
        if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_POST, 1L)) != CURLE_OK ) {
            throw std::runtime_error(std::format("cURL: set POST: {}", ::curl_easy_strerror(res)));
        }
        
        TrioPOD<const char *, size_t, size_t> readDataUser;
        if (postData && postDataSize) {
            readDataUser.first = static_cast<const char *>(postData);
            readDataUser.second = postDataSize;
            readDataUser.third = 0;
            
            if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_READFUNCTION, CURLRequestReadDataCallback)) != CURLE_OK ) {
                throw std::runtime_error(std::format("cURL: set read function: {}", ::curl_easy_strerror(res)));
            }
            
            if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_READDATA, static_cast<void *>(&readDataUser))) != CURLE_OK ) {
                throw std::runtime_error(std::format("cURL: set read data: {}", ::curl_easy_strerror(res)));
            }
        }
        
        auto movedData = static_cast<std::shared_ptr<crypto::ZeroFillDataVector> &&>(responceData);
        if (movedData) {
            movedData->clear();
        } else {
            movedData = std::make_shared<crypto::ZeroFillDataVector>();
        }
        
        if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_WRITEDATA, movedData.get())) != CURLE_OK ) {
            throw std::runtime_error(std::format("cURL: set write data: {}", ::curl_easy_strerror(res)));
        }
        
        if ( (res = ::curl_easy_perform(base.first.get())) != CURLE_OK ) {
            throw std::runtime_error(std::format("cURL: perform: {}", ::curl_easy_strerror(res)));
        }
        
        return movedData;
    }
    
    static std::atomic<int> _globalCURLInitCounter = 0;
    
    void CURLRequest::globalInit() noexcept {
        if (_globalCURLInitCounter.fetch_add(1) == 0) {
            ::curl_global_init(CURL_GLOBAL_ALL);
        }
    }
    
    void CURLRequest::globalDeinit() noexcept {
        switch (_globalCURLInitCounter.fetch_sub(1)) {
            case 0:
                _globalCURLInitCounter = 0;
                break;
                
            case 1:
                ::curl_global_cleanup();
                break;
                
            default:
                break;
        }
    }

    ///@link https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Headers/User-Agent
    static crypto::ZeroFillString CURLRequestUASystemInformation() {
        crypto::ZeroFillStringStream stream;
#if defined(HAVE_SYS_UTSNAME_H)
        struct utsname systemInfo;
        if (::uname(&systemInfo) == 0) {
            stream << " (" << systemInfo.sysname << ' ' << systemInfo.release << "; " << systemInfo.machine;
#  if defined(__GNUC__) && defined(__VERSION__)
            stream << "; GCC " __VERSION__;
#  endif
            stream << ')';
            return stream.str();
        }
#endif
        
#if defined(BUILD_SYSTEM_NAME) && defined(BUILD_SYSTEM_PROCESSOR)
        stream << " (" BUILD_SYSTEM_NAME "; " BUILD_SYSTEM_PROCESSOR;
#  if defined(__GNUC__) && defined(__VERSION__)
        stream << "; GCC " __VERSION__;
#  endif
        stream << ')';
        return stream.str();
#elif defined(__GNUC__) && defined(__VERSION__)
        stream << " (GCC " __VERSION__ ")";
        return stream.str();
#else
        return {};
#endif
    }
    
    crypto::ZeroFillString CURLRequest::generateUserAgent() {
        crypto::ZeroFillStringStream stream;
#if defined(PROJECT_NAME)
        stream << PROJECT_NAME;
#else
        stream << "NGA";
#endif
        
#if defined(CLIENT_VERSION_MAJOR) && defined(CLIENT_VERSION_MINOR) && defined(CLIENT_VERSION_PATCH)
        stream << '/' << CLIENT_VERSION_MAJOR << '.' << CLIENT_VERSION_MINOR << '.' << CLIENT_VERSION_PATCH;
#else
        stream << "/0.0.1";
#endif
        
        stream << CURLRequestUASystemInformation();
        
        auto * curlInfo = ::curl_version_info(CURLVERSION_NOW);
        if (curlInfo) {
            if (curlInfo->version) {
                stream << ' ' << "cURL" << '/' << curlInfo->version;
            }
            if (curlInfo->ssl_version) {
                stream << ' ' << curlInfo->ssl_version;
            }
        }
        
        return stream.str();
    }
    
} // namespace nga
