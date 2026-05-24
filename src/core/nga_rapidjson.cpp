/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <cstddef>

#include "nga_rapidjson.hpp"

namespace RAPIDJSON_NAMESPACE {
    
    const Value emptyArrayValue(kArrayType);
    const Value emptyObjectValue(kObjectType);
    
    const char * NGA_NONNULL findCString(const Value & obj, const char * NGA_NONNULL key) {
        const auto it = obj.FindMember(key);
        if (it != obj.MemberEnd() && it->value.IsString()) {
            return it->value.GetString() ?: emptyCString;
        }
        throw std::runtime_error(std::format("JSON: string not found: {}", key));
    }
    
    const char * NGA_NONNULL findCString(const Value & obj,
                                         const char * NGA_NONNULL key,
                                         const char * NGA_NONNULL defValue) noexcept {
        const auto it = obj.FindMember(key);
        if (it != obj.MemberEnd() && it->value.IsString()) {
            return it->value.GetString() ?: emptyCString;
        }
        return defValue;
    }
    
    const Value & findArray(const Value & obj, const char * NGA_NONNULL key) {
        const auto it = obj.FindMember(key);
        if (it != obj.MemberEnd() && it->value.IsArray()) {
            return it->value;
        }
        throw std::runtime_error(std::format("JSON: array not found: {}", key));
    }
    
    const Value & findArray(const Value & obj, const char * NGA_NONNULL key, const Value & defValue) noexcept {
        const auto it = obj.FindMember(key);
        if (it != obj.MemberEnd() && it->value.IsArray()) {
            return it->value;
        }
        return defValue;
    }
    
    const Value & findObject(const Value & obj, const char * NGA_NONNULL key) {
        const auto it = obj.FindMember(key);
        if (it != obj.MemberEnd() && it->value.IsObject()) {
            return it->value;
        }
        throw std::runtime_error(std::format("JSON: object not found: {}", key));
    }
    
    const Value & findObject(const Value & obj, const char * NGA_NONNULL key, const Value & defValue) noexcept {
        const auto it = obj.FindMember(key);
        if (it != obj.MemberEnd() && it->value.IsObject()) {
            return it->value;
        }
        return defValue;
    }
    
    bool findBool(const Value & obj, const char * NGA_NONNULL key) {
        const auto it = obj.FindMember(key);
        if (it != obj.MemberEnd() && it->value.IsBool()) {
            return it->value.GetBool();
        }
        throw std::runtime_error(std::format("JSON: boolean not found: {}", key));
    }
    
    bool findBool(const Value & obj, const char * NGA_NONNULL key, const bool defValue) noexcept {
        const auto it = obj.FindMember(key);
        if (it != obj.MemberEnd() && it->value.IsBool()) {
            return it->value.GetBool();
        }
        return defValue;
    }
    
} // namespace RAPIDJSON_NAMESPACE
