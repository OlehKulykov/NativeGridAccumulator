/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __R2D9_CURL_REQUEST_HPP__
#define __R2D9_CURL_REQUEST_HPP__ 1

#include <memory>
#include <vector>

#if (__cplusplus >= 202002L)
#  include <future>
#endif

#include "r2d9_data.hpp"

namespace r2d9 {
    
    class CURLRequest {
    public:
        static std::shared_ptr<Data> GET(const char * R2D9_NONNULL url,
                                         const std::vector<const char *> & headers,
                                         std::shared_ptr<Data> && responceData = nullptr);
        
        static std::shared_ptr<Data> POST(const char * R2D9_NONNULL url,
                                          const std::vector<const char *> & headers,
                                          const std::shared_ptr<Data> & postData,
                                          std::shared_ptr<Data> && responceData = nullptr);
        
#if (__cplusplus >= 202002L)
        static std::future<std::shared_ptr<Data> > asyncGET(const char * R2D9_NONNULL url,
                                                            const std::vector<const char *> & headers,
                                                            std::shared_ptr<Data> && data = nullptr);
#endif
        
        static void globalInit() noexcept;
        static void globalDeinit() noexcept;
    };
    
} // namespace r2d9

#endif //!__R2D9_CURL_REQUEST_HPP__
