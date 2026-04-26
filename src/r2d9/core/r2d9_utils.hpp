/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __R2D9_UTILS_HPP__
#define __R2D9_UTILS_HPP__ 1

#include <algorithm>
#include <cstdint>
#include <type_traits>

#include "../r2d9.hpp"

namespace r2d9 {
    
    template<typename T>
    inline constexpr T roundUpToMultiple(const T value) noexcept {
        static_assert(std::is_integral<T>::value, "!int");
        static_assert(std::is_unsigned<T>::value, "!uint");
        static_assert(sizeof(T) != 0, "T==0");
        
        constexpr const T alignment = sizeof(T);
        return (((value + alignment - 1) / alignment) * alignment);
    }
    
    template<typename T>
    inline constexpr T roundUpToMultiple(const T value, const T alignment) noexcept {
        static_assert(std::is_integral<T>::value, "!int");
        static_assert(std::is_unsigned<T>::value, "!uint");
        
        if (alignment == 0) {
            return value;
        }
        
        return (((value + alignment - 1) / alignment) * alignment);
    }
    
    template<typename T>
    inline constexpr bool isPowerOfTwo(const T value) noexcept {
        static_assert(std::is_integral<T>::value, "!int");
        static_assert(std::is_unsigned<T>::value, "!uint");
        
        return ((value > 0) && ((value & (value - 1)) == 0));
    }
    
    template<typename T>
    bool isPointerAligned(const void * R2D9_NONNULL ptr, const T alignment = alignof(T)) noexcept {
        if (!ptr) {
            return true;
        }
        if (alignment == 0) {
            return false;
        }
        
        const uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
        return isPowerOfTwo<T>(alignment) ? ((address & (alignment - 1)) == 0) : ((address % alignment) == 0);
    }
    
} // namespace r2d9

#endif //!__R2D9_UTILS_HPP__
