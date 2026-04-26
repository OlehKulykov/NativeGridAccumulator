/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <cstring>

#include "r2d9_http.hpp"

#include "r2d9_c_string.h"

namespace r2d9 {
namespace http {
    
    const char * R2D9_NONNULL const contentTypeAppOctetStream   = "application/octet-stream";
    const char * R2D9_NONNULL const contentTypeAppJson          = "application/json";
    const char * R2D9_NONNULL const contentTypeTextHtmlUtf8     = "text/html; charset=utf-8";
    
    const char * R2D9_NONNULL contentTypeToString(const ContentType contentType) noexcept {
        switch (contentType) {
            case ContentType::AppOctetStream:   return contentTypeAppOctetStream;
            case ContentType::AppJson:          return contentTypeAppJson;
            case ContentType::TextHtmlUtf8:     return contentTypeTextHtmlUtf8;
        }
        return emptyCString;
    }
    
    const char * R2D9_NONNULL const contentEncodingGZip     = "gzip";
    const char * R2D9_NONNULL const contentEncodingDeflate  = "deflate";
    const char * R2D9_NONNULL const contentEncodingBr       = "br";
    const char * R2D9_NONNULL const contentEncodingCompress = "compress";
    const char * R2D9_NONNULL const contentEncodingZstd     = "zstd";
    const char * R2D9_NONNULL const contentEncodingDcb      = "dcb";
    const char * R2D9_NONNULL const contentEncodingDcz      = "dcz";
    
    const char * R2D9_NULLABLE contentEncodingToString(const ContentEncoding contentEncoding) noexcept {
        switch (contentEncoding) {
            case ContentEncoding::GZip:     return contentEncodingGZip;
            case ContentEncoding::Zstd:     return contentEncodingZstd;
            case ContentEncoding::Deflate:  return contentEncodingDeflate;
            case ContentEncoding::Br:       return contentEncodingBr;
            case ContentEncoding::Compress: return contentEncodingCompress;
            case ContentEncoding::Dcb:      return contentEncodingDcb;
            case ContentEncoding::Dcz:      return contentEncodingDcz;
            case ContentEncoding::Raw:      break;
        }
        return nullptr;
    }
    
    const char * R2D9_NONNULL const headerAcceptCharset     = "Accept-Charset";
    const char * R2D9_NONNULL const headerAcceptLanguage    = "Accept-Language";
    const char * R2D9_NONNULL const headerAuthorization     = "Authorization";
    const char * R2D9_NONNULL const headerContentEncoding   = "Content-Encoding";
    const char * R2D9_NONNULL const headerContentType       = "Content-Type";
    const char * R2D9_NONNULL const headerContentLength     = "Content-Length";
    const char * R2D9_NONNULL const headerDate              = "Date";
    const char * R2D9_NONNULL const headerETag              = "ETag";
    const char * R2D9_NONNULL const headerExpires           = "Expires";
    const char * R2D9_NONNULL const headerUserAgent         = "User-Agent";
    const char * R2D9_NONNULL const headerLocation          = "Location";
    
    //TODO: ::sort + ::bsearch
    struct TmpHeaderInfo final {
        const char * s;
        Header h;
    };
    
    static const TmpHeaderInfo headerInfos[11] = {
        { headerAcceptCharset, Header::AcceptCharset },
        { headerAcceptLanguage, Header::AcceptLanguage },
        { headerAuthorization, Header::Authorization },
        { headerDate, Header::Date },
        { headerETag, Header::ETag },
        { headerExpires, Header::Expires },
        { headerUserAgent, Header::UserAgent },
        { headerLocation, Header::Location },
        { headerContentEncoding, Header::ContentEncoding },
        { headerContentType, Header::ContentType },
        { headerContentLength, Header::ContentLength }
    };
    
    const char * R2D9_NULLABLE headerToString(const Header header) noexcept {
        for (size_t i = 0; i < 11; i++) {
            if (header == headerInfos[i].h) {
                return headerInfos[i].s;
            }
        }
        return nullptr;
    }
    
    Header headerFromString(const char * R2D9_NULLABLE s) noexcept {
        if (s) {
            for (size_t i = 0; i < 11; i++) {
                if (::strcasecmp(s, headerInfos[i].s) == 0) {
                    return headerInfos[i].h;
                }
            }
        }
        return Header::None;
    }

} // namespace http
} // namespace r2d9
