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

#define __R2D9_FILE__ "cURL"

#include "r2d9_curl_request.hpp"
#include "r2d9_fixed_string_stream.hpp"
#include "r2d9_scope_guard.hpp"

#include <curl/curl.h>

namespace r2d9 {
    
    struct CURLDeleter final {
        void operator()(CURL * curl) const {
            if (curl) {
                ::curl_easy_cleanup(curl);
            }
        }
    };
    
    struct CURLSListDeleter final {
        void operator()(struct curl_slist * sList) const {
            if (sList) {
                ::curl_slist_free_all(sList);
            }
        }
    };
    
    static size_t CURLWriteCallback(char * contents, size_t size, size_t nmemb, void * userp) noexcept {
        Data * data = static_cast<Data *>(userp);
        const size_t writeSize = size * nmemb;
        try {
            data->append(contents, writeSize);
        } catch (...) {
            return 0;
        }
        return writeSize;
    }
    
    typedef std::unique_ptr<struct curl_slist, CURLSListDeleter> CURLSListUPtr;
    typedef std::unique_ptr<CURL, CURLDeleter> CURLUPtr;
    typedef std::pair<CURLUPtr, CURLSListUPtr> CURLBaseRequest;
    
    static CURLBaseRequest CURLCreateCURLBaseRequest(const char * R2D9_NONNULL url,
                                                     const std::vector<const char *> & headers) {
        FixedStringStream<255, false> stream;
        
        ///@link https://curl.se/libcurl/c/httpcustomheader.html
        CURLSListUPtr headersSList;
        {
            struct curl_slist * prevList = nullptr;
            for (size_t i = 0, n = headers.size(); i < n; i++) {
                struct curl_slist * newList = ::curl_slist_append(prevList, headers[i]);
                if (newList) {
                    prevList = newList;
                } else {
                    if (prevList) {
                        ::curl_slist_free_all(prevList);
                    }
                    stream << "Set headers, URL: " << url;
                    throw std::runtime_error(stream);
                }
            }
            headersSList = CURLSListUPtr(prevList);
        }
        
        CURLUPtr curl(::curl_easy_init());
        if (!curl) {
            stream << "GET init, URL: " << url;
            throw std::runtime_error(stream);
        }
        
        CURLcode res;
        if (headersSList && ((res = curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headersSList.get())) != CURLE_OK) ) {
            stream << "Set headers: " << ::curl_easy_strerror(res) << ", URL: " << url;
            throw std::runtime_error(stream);
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_URL, url)) != CURLE_OK ) {
            stream << "URL: " << ::curl_easy_strerror(res) << ", URL: " << url;
            throw std::runtime_error(stream);
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 30)) != CURLE_OK ) {
            stream << "Timeout: " << ::curl_easy_strerror(res) << ", URL: " << url;
            throw std::runtime_error(stream);
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, CURLWriteCallback)) != CURLE_OK ) {
            stream << "Set write: " << ::curl_easy_strerror(res) << ", URL: " << url;
            throw std::runtime_error(stream);
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 1L)) != CURLE_OK ) {
            stream << "SSL verify peer: " << ::curl_easy_strerror(res) << ", URL: " << url;
            throw std::runtime_error(stream);
        }
        
        if ( (res = curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYHOST, 2L)) != CURLE_OK ) {
            stream << "SSL verify host: " << ::curl_easy_strerror(res) << ", URL: " << url;
            throw std::runtime_error(stream);
        }
        
        return CURLBaseRequest(static_cast<CURLUPtr &&>(curl), static_cast<CURLSListUPtr &&>(headersSList));
    }
    
    std::shared_ptr<Data> CURLRequest::GET(const char * R2D9_NONNULL url,
                                           const std::vector<const char *> & headers,
                                           std::shared_ptr<Data> && responceData) {
        auto movedData = static_cast<std::shared_ptr<Data> &&>(responceData);
        if (!movedData) {
            movedData = std::make_shared<Data>();
        }
        movedData->reset();
        
        CURLRequest::globalInit();
        
        ScopeGuard lock([] {
            CURLRequest::globalDeinit();
        });
        
        CURLBaseRequest base = CURLCreateCURLBaseRequest(url, headers);
        
        CURLcode res;
        FixedStringStream<255, false> stream;
        
        if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_WRITEDATA, movedData.get())) != CURLE_OK ) {
            stream << "Write data: " << ::curl_easy_strerror(res) << ", URL: " << url;
            throw std::runtime_error(stream);
        }
        
        if ( (res = ::curl_easy_perform(base.first.get())) != CURLE_OK ) {
            stream << "Perform: " << ::curl_easy_strerror(res) << ", URL: " << url;
            throw std::runtime_error(stream);
        }
        
        return movedData;
    }
    
    std::shared_ptr<Data> CURLRequest::POST(const char * R2D9_NONNULL url,
                                            const std::vector<const char *> & headers,
                                            const std::shared_ptr<Data> & postData,
                                            std::shared_ptr<Data> && responceData) {
        auto movedData = static_cast<std::shared_ptr<Data> &&>(responceData);
        if (!movedData) {
            movedData = std::make_shared<Data>();
        }
        movedData->reset();
        
        CURLRequest::globalInit();
        
        ScopeGuard lock([] {
            CURLRequest::globalDeinit();
        });
        
        CURLBaseRequest base = CURLCreateCURLBaseRequest(url, headers);
        
        CURLcode res;
        FixedStringStream<255, false> stream;
        
        if (postData) {
            if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_POSTFIELDSIZE, static_cast<long>(postData->size()))) != CURLE_OK ) {
                stream << "Set POST data size: " << ::curl_easy_strerror(res) << ", URL: " << url;
                throw std::runtime_error(stream);
            }
            if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_POSTFIELDS, postData->data())) != CURLE_OK ) {
                stream << "Set POST data: " << ::curl_easy_strerror(res) << ", URL: " << url;
                throw std::runtime_error(stream);
            }
        } else {
            if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_POST, 1L)) != CURLE_OK ) {
                stream << "Set POST: " << ::curl_easy_strerror(res) << ", URL: " << url;
                throw std::runtime_error(stream);
            }
            if ( (res =  curl_easy_setopt(base.first.get(), CURLOPT_POSTFIELDSIZE, 0L)) != CURLE_OK ) {
                stream << "Set POST empty data size: " << ::curl_easy_strerror(res) << ", URL: " << url;
                throw std::runtime_error(stream);
            }
        }
        
        if ( (res = curl_easy_setopt(base.first.get(), CURLOPT_WRITEDATA, movedData.get())) != CURLE_OK ) {
            stream << "Write data: " << ::curl_easy_strerror(res) << ", URL: " << url;
            throw std::runtime_error(stream);
        }
        
        if ( (res = ::curl_easy_perform(base.first.get())) != CURLE_OK ) {
            stream << "Perform: " << ::curl_easy_strerror(res) << ", URL: " << url;
            throw std::runtime_error(stream);
        }
        
        return movedData;
    }
    
#if (__cplusplus >= 202002L)
    std::future<std::shared_ptr<Data> > CURLRequest::asyncGET(const char * R2D9_NONNULL url,
                                                              const std::vector<const char *> & headers,
                                                              std::shared_ptr<Data> && data) {
        return std::async(std::launch::async, [url, headers, movedData = static_cast<std::shared_ptr<Data> &&>(data)]() mutable {
            return CURLRequest::GET(url, headers, static_cast<std::shared_ptr<Data> &&>(movedData));
        });
    }
#endif
    
    static std::atomic<int> _globalCURLInitCounter = 0;
    
    void CURLRequest::globalInit() noexcept {
        if (_globalCURLInitCounter.fetch_add(1) == 0) {
            ::curl_global_init(CURL_GLOBAL_ALL);
        }
    }
    
    void CURLRequest::globalDeinit() noexcept {
        switch (_globalCURLInitCounter.fetch_sub(1)) {
            case 0: _globalCURLInitCounter = 0; break;
            case 1: ::curl_global_cleanup(); break;
            default: break;
        }
    }
    
} // namespace r2d9
