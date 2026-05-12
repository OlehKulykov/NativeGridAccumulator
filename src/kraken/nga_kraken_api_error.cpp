/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <string>
#include <cstring>

#include "nga_kraken_api_error.hpp"

#include "../core/nga_c_string.h"

namespace nga {
namespace kraken {
    
    ///@link https://docs.kraken.com/api/docs/guides/spot-rest-intro
    void APIError::initFromStr(const char * str) {
        _what = str;
        
        APIErrorSeverity severity;
        APIErrorCategory category;
        
        if (*str == 'E' || *str == 'e') {
            severity = APIErrorSeverity::error;
        } else if (*str == 'W' || *str == 'w') {
            severity = APIErrorSeverity::warning;
        } else {
            return;
        }
        if (++str == 0) {
            return;
        }
        
        if (::strncasecmp(str, "general", 7) == 0) {
            category = APIErrorCategory::general;
            str += 7;
        } else if (::strncasecmp(str, "auth", 4) == 0) {
            category = APIErrorCategory::auth;
            str += 4;
        } else if (::strncasecmp(str, "api", 3) == 0) {
            category = APIErrorCategory::api;
            str += 3;
        } else if (::strncasecmp(str, "query", 5) == 0) {
            category = APIErrorCategory::query;
            str += 5;
        } else if (::strncasecmp(str, "order", 5) == 0) {
            category = APIErrorCategory::order;
            str += 5;
        } else if (::strncasecmp(str, "trade", 5) == 0) {
            category = APIErrorCategory::trade;
            str += 5;
        } else if (::strncasecmp(str, "funding", 7) == 0) {
            category = APIErrorCategory::funding;
            str += 7;
        } else if (::strncasecmp(str, "service", 7) == 0) {
            category = APIErrorCategory::service;
            str += 7;
        } else {
            return;
        }
        if (*str++ != ':') {
            return;
        }
        
        // "<severity><category>:"
        _severity = severity;
        _category = category;
        
        if (*str == 0) {
            return;
        }
        while (*str == ' ') {
            str++;
        }
        if (*str != 0) {
            _description = str;
        }
    }
    
    const char * APIError::description() const noexcept {
        return _description.c_str();
    }
    
    APIErrorSeverity APIError::severity() const noexcept {
        return _severity;
    }
    
    APIErrorCategory APIError::category() const noexcept {
        return _category;
    }
    
    const char * APIError::what() const noexcept {
        return _what.c_str();
    }
    
    APIError & APIError::operator=(APIError && err) {
        _what = std::move(err._what);
        _description = std::move(err._description);
        _severity = err._severity;
        _category = err._category;
        err._severity = APIErrorSeverity{0};
        err._category = APIErrorCategory{0};
        return *this;
    }
    
    APIError & APIError::operator=(const APIError & err) {
        _severity = err._severity;
        _category = err._category;
        _what.clear();
        _description.clear();
        try {
            _what = err._what;
            _description = err._description;
        } catch (...) {
            // do nothing, noexcept '='
        }
        return *this;
    }
    
    APIError::APIError(APIError && err) : std::runtime_error(emptyCString) {
        *this = std::move(err);
    }
    
    APIError::APIError(const APIError & err) : std::runtime_error(emptyCString) {
        *this = err;
    }
    
    APIError::APIError(const char * someString) : std::runtime_error(emptyCString) {
        if (someString) {
            try {
                initFromStr(someString);
            } catch (...) {
                // do nothing, noexcept init
            }
        }
    }
    
} // namespace kraken
} // namespace nga
