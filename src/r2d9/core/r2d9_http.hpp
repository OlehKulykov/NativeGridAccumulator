/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __R2D9_HTTP_HPP__
#define __R2D9_HTTP_HPP__ 1

#include <cstdarg>

#include "../r2d9.hpp"

namespace r2d9 {
namespace http {
    
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) contentTypeAppOctetStream;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) contentTypeAppJson;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) contentTypeTextHtmlUtf8;
    
    enum class ContentType : uint8_t {
        AppOctetStream  = 0,
        AppJson         = 1,
        TextHtmlUtf8    = 2
    };
    
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL) contentTypeToString(const ContentType contentType) noexcept;
    
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) contentEncodingGZip;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) contentEncodingDeflate;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) contentEncodingBr;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) contentEncodingCompress;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) contentEncodingZstd;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) contentEncodingDcb;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) contentEncodingDcz;
    
    enum class ContentEncoding : uint8_t {
        Raw      = 0,
        GZip     = 1,
        Zstd     = 2,
        Deflate  = 3,
        Br       = 4,
        Compress = 5,
        Dcb      = 6,
        Dcz      = 7
    };
    
    R2D9_CPP_API_PRIVATE(const char * R2D9_NULLABLE) contentEncodingToString(const ContentEncoding contentEncoding) noexcept;
    
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerAccept;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerAcceptCharset;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerAcceptLanguage;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerAuthorization;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerContentEncoding;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerContentType;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerContentLength;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerDate;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerETag;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerExpires;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerUserAgent;
    R2D9_CPP_API_PRIVATE(const char * R2D9_NONNULL const) headerLocation;
    
    enum class Header : uint8_t {
        Accept          = 1,
        AcceptCharset   = 2,
        AcceptLanguage  = 3,
        Authorization   = 4,
        Date            = 5,
        ETag            = 6,
        Expires         = 7,
        UserAgent       = 8,
        Location        = 9,
        ContentEncoding = 10,
        ContentType     = 11,
        ContentLength   = 12
    };
    
    R2D9_CPP_API_PRIVATE(const char * R2D9_NULLABLE) headerToString(const Header header) noexcept;
    R2D9_CPP_API_PRIVATE(Header) headerFromString(const char * R2D9_NULLABLE s) noexcept;

} // namespace http
} // namespace r2d9

#endif //!__R2D9_HTTP_HPP__
