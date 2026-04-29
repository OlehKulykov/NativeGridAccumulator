/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <atomic>
#include <cstddef>
#include <algorithm>

#define __R2D9_FILE__ "cURL"

#include "r2d9/core/r2d9_fixed_string_stream.hpp"
#include "r2d9/core/r2d9_trio.hpp"
#include "r2d9/core/r2d9_scope_guard.hpp"

#include <curl/curl.h>

#include "nga_curl_request.hpp"

namespace nga {
    
    using namespace r2d9;
    
    struct CURLDeleter final {
        void operator()(CURL * R2D9_NULLABLE curl) const noexcept {
            if (curl) {
                ::curl_easy_cleanup(curl);
            }
        }
    };
    
    struct CURLSListDeleter final {
        void operator()(struct curl_slist * R2D9_NULLABLE sList) const noexcept {
            if (sList) {
                ::curl_slist_free_all(sList);
            }
        }
    };
    
    typedef std::unique_ptr<CURL, CURLDeleter> CURLUPtr;
    typedef std::unique_ptr<struct curl_slist, CURLSListDeleter> CURLSListUPtr;
    typedef std::pair<CURLUPtr, CURLSListUPtr> CURLBaseRequest;
    
    static size_t CURLRequestWriteDataCallback(char * R2D9_NULLABLE contents, size_t size, size_t nmemb, void * R2D9_NULLABLE userp) noexcept {
        size_t writeSize = size * nmemb;
        try {
            if (contents && writeSize) {
                auto * user = static_cast<ZeroFillDataVector *>(userp);
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
    
    static size_t CURLRequestReadDataCallback(char * R2D9_NULLABLE contents, size_t size, size_t nmemb, void * R2D9_NULLABLE userp) noexcept {
        auto * user = static_cast<Trio<const char *, size_t, size_t> *>(userp);
        const size_t readSize = std::min(user->second - user->third, size * nmemb);
        if (contents && readSize) {
            ::memcpy(contents, user->first + user->third, readSize);
            user->third += readSize;
            return readSize;
        }
        return 0;
    }
    
    static CURLBaseRequest CURLRequestCreateBaseRequest(const char * R2D9_NONNULL url,
                                                        const char * R2D9_NULLABLE * R2D9_NULLABLE headers,
                                                        const size_t headersCount) {
        FixedStringStream<255> stream;
        ScopeGuard lock([&] {
            stream.clear(true);
        });
        
        ///@link https://curl.se/libcurl/c/httpcustomheader.html
        CURLSListUPtr headersSList;
        {
            struct curl_slist * prevList = nullptr;
            for (size_t i = 0; i < headersCount; i++) {
                struct curl_slist * newList = ::curl_slist_append(prevList, headers[i]);
                if (newList) {
                    prevList = newList;
                } else {
                    if (prevList) {
                        ::curl_slist_free_all(prevList);
                    }
                    stream << "Set headers";
                    throw std::runtime_error(stream);
                }
            }
            headersSList = CURLSListUPtr(prevList);
        }
        
        CURLUPtr curl(::curl_easy_init());
        if (!curl) {
            stream << "GET init";
            throw std::runtime_error(stream);
        }
        
        CURLcode res;
        if (headersSList && ((res = curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headersSList.get())) != CURLE_OK) ) {
            stream << "Set headers: " << ::curl_easy_strerror(res);
            throw std::runtime_error(stream);
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_URL, url)) != CURLE_OK ) {
            stream << "URL: " << ::curl_easy_strerror(res);
            throw std::runtime_error(stream);
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 30)) != CURLE_OK ) {
            stream << "Timeout: " << ::curl_easy_strerror(res);
            throw std::runtime_error(stream);
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 1L)) != CURLE_OK ) {
            stream << "SSL verify peer: " << ::curl_easy_strerror(res);
            throw std::runtime_error(stream);
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYHOST, 2L)) != CURLE_OK ) {
            stream << "SSL verify host: " << ::curl_easy_strerror(res);
            throw std::runtime_error(stream);
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, CURLRequestWriteDataCallback)) != CURLE_OK ) {
            stream << "Set write: " << ::curl_easy_strerror(res);
            throw std::runtime_error(stream);
        }
        
        return CURLBaseRequest(static_cast<CURLUPtr &&>(curl), static_cast<CURLSListUPtr &&>(headersSList));
    }
    
    std::shared_ptr<ZeroFillDataVector> CURLRequest::GET(const char * R2D9_NONNULL url,
                                                         const char * R2D9_NULLABLE headers[R2D9_NONNULL],
                                                         const size_t headersCount,
                                                         std::shared_ptr<ZeroFillDataVector> && responceData) {
        CURLRequest::globalInit();
        FixedStringStream<255> stream;
        ScopeGuard lock([&] {
            stream.clear(true);
            CURLRequest::globalDeinit();
        });
        
        auto base = CURLRequestCreateBaseRequest(url, headers, headersCount);
        
        auto movedData = static_cast<std::shared_ptr<ZeroFillDataVector> &&>(responceData);
        if (!movedData) {
            movedData = std::make_shared<ZeroFillDataVector>();
        }
        
        CURLcode res;
        if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_WRITEDATA, movedData.get())) != CURLE_OK ) {
            stream << "Write data: " << ::curl_easy_strerror(res);
            throw std::runtime_error(stream);
        }
        
        if ( (res = ::curl_easy_perform(base.first.get())) != CURLE_OK ) {
            stream << "Perform: " << ::curl_easy_strerror(res);
            throw std::runtime_error(stream);
        }
        
        return movedData;
    }
    
    std::shared_ptr<ZeroFillDataVector> CURLRequest::POST(const char * R2D9_NONNULL url,
                                                          const char * R2D9_NULLABLE headers[R2D9_NONNULL],
                                                          const size_t headersCount,
                                                          const void * R2D9_NULLABLE postData,
                                                          const size_t postDataSize,
                                                          std::shared_ptr<ZeroFillDataVector> && responceData) {
        CURLRequest::globalInit();
        FixedStringStream<255> stream;
        ScopeGuard lock([&] {
            stream.clear(true);
            CURLRequest::globalDeinit();
        });
        
        auto base = CURLRequestCreateBaseRequest(url, headers, headersCount);
        
        CURLcode res;
        if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_POST, 1L)) != CURLE_OK ) {
            stream << "Set POST: " << ::curl_easy_strerror(res);
            throw std::runtime_error(stream);
        }
        
        Trio<const char *, size_t, size_t> readDataUser;
        if (postData && postDataSize) {
            readDataUser.first = static_cast<const char *>(postData);
            readDataUser.second = postDataSize;
            readDataUser.third = 0;
            
            if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_READFUNCTION, CURLRequestReadDataCallback)) != CURLE_OK ) {
                stream << "Read function: " << ::curl_easy_strerror(res);
                throw std::runtime_error(stream);
            }
            
            if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_READDATA, static_cast<void *>(&readDataUser))) != CURLE_OK ) {
                stream << "Read data: " << ::curl_easy_strerror(res);
                throw std::runtime_error(stream);
            }
        }
        
        auto movedData = static_cast<std::shared_ptr<ZeroFillDataVector> &&>(responceData);
        if (!movedData) {
            movedData = std::make_shared<ZeroFillDataVector>();
        }
        
        if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_WRITEDATA, movedData.get())) != CURLE_OK ) {
            stream << "Write data: " << ::curl_easy_strerror(res);
            throw std::runtime_error(stream);
        }
        
        if ( (res = ::curl_easy_perform(base.first.get())) != CURLE_OK ) {
            stream << "Perform: " << ::curl_easy_strerror(res);
            throw std::runtime_error(stream);
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
    
} // namespace nga
