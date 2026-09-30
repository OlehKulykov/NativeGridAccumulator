/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_HTTP_HPP__
#define __NGA_HTTP_HPP__ 1

#include <cstdarg>

#include "../nga.hpp"

namespace nga {
namespace http {
    
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) contentTypeAppOctetStream;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) contentTypeAppJson;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) contentTypeTextHtmlUtf8;
    
    enum class ContentType : uint8_t {
        AppOctetStream  = 0,
        AppJson         = 1,
        TextHtmlUtf8    = 2
    };
    
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL) contentTypeToString(const ContentType contentType) noexcept;
    
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) contentEncodingGZip;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) contentEncodingDeflate;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) contentEncodingBr;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) contentEncodingCompress;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) contentEncodingZstd;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) contentEncodingDcb;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) contentEncodingDcz;
    
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
    
    NGA_CPP_API_PRIVATE(const char * NGA_NULLABLE) contentEncodingToString(const ContentEncoding contentEncoding) noexcept;
    
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerAccept;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerAcceptCharset;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerAcceptLanguage;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerAuthorization;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerContentEncoding;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerContentType;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerContentLength;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerCacheControl;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerDate;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerETag;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerExpires;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerUserAgent;
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL const) headerLocation;
    
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
    
    NGA_CPP_API_PRIVATE(const char * NGA_NULLABLE) headerToString(const Header header) noexcept;
    NGA_CPP_API_PRIVATE(Header) headerFromString(const char * NGA_NULLABLE s) noexcept;

} // namespace http
} // namespace nga

#endif //!__NGA_HTTP_HPP__
