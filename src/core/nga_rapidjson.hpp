/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_RAPIDJSON_HPP__
#define __NGA_RAPIDJSON_HPP__ 1

#include <stdexcept>
#include <cstdio>

#define RAPIDJSON_NAMESPACE ngarj
#define RAPIDJSON_NO_SIZETYPEDEFINE 1
#define RAPIDJSON_HAS_STDSTRING 1
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 1
#define RAPIDJSON_HAS_CXX11_NOEXCEPT 1

namespace RAPIDJSON_NAMESPACE {
    typedef ::size_t SizeType;
}

#include <rapidjson/document.h>

#include "nga_c_string.h"

namespace RAPIDJSON_NAMESPACE {
    
    NGA_CPP_API_PRIVATE(const Value) emptyArrayValue;
    NGA_CPP_API_PRIVATE(const Value) emptyObjectValue;
    
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL) findCString(const Value & obj, const char * NGA_NONNULL key);
    NGA_CPP_API_PRIVATE(const char * NGA_NONNULL) findCString(const Value & obj, const char * NGA_NONNULL key, const char * NGA_NONNULL defValue) noexcept;
    
    NGA_CPP_API_PRIVATE(const Value &) findArray(const Value & obj, const char * NGA_NONNULL key);
    NGA_CPP_API_PRIVATE(const Value &) findArray(const Value & obj, const char * NGA_NONNULL key, const Value & defValue) noexcept;
    
    NGA_CPP_API_PRIVATE(const Value &) findObject(const Value & obj, const char * NGA_NONNULL key);
    NGA_CPP_API_PRIVATE(const Value &) findObject(const Value & obj, const char * NGA_NONNULL key, const Value & defValue) noexcept;
    
    template<typename T>
    T findString(const Value & obj, const char * NGA_NONNULL key) {
        return T(findCString(obj, key));
    }
    
    template<typename T>
    T findString(const Value & obj, const char * NGA_NONNULL key, const char * NGA_NONNULL defValue) noexcept {
        return T(findCString(obj, key, defValue));
    }
    
    template<typename T>
    T findNumber(const Value & obj, const char * NGA_NONNULL key) {
        const auto it = obj.FindMember(key);
        if (it != obj.MemberEnd() && it->value.IsNumber()) {
            return it->value.Get<T>();
        }
        char reason[128];
        ::snprintf(reason, 128, "Number not found %s", key);
        throw std::runtime_error(reason);
    }
    
    template<typename T>
    T findNumber(const Value & obj, const char * NGA_NONNULL key, const T & defValue) noexcept {
        const auto it = obj.FindMember(key);
        if (it != obj.MemberEnd() && it->value.IsNumber()) {
            return it->value.Get<T>();
        }
        return defValue;
    }
}

#endif //!__NGA_RAPIDJSON_HPP__
