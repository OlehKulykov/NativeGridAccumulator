/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __R2D9_CRYPTO_HPP__
#define __R2D9_CRYPTO_HPP__ 1

#include <memory>
#include <array>
#include <vector>
#include <stdexcept>
#include <cstring>

#include "r2d9_fixed_string_stream.hpp"

namespace r2d9 {
namespace crypto {
    
    template<class T>
    inline void zeroFill(T & container) noexcept {
        auto * containerData = container.data();
        if (containerData) {
            ::memset(containerData, 0, container.size());
        }
    }
    
    template<typename T>
    class ZeroFillAllocator { // No 'final' - gcc bug
    public:
        using value_type = T;
        
        T * R2D9_NONNULL allocate(std::size_t n) {
            T * p = static_cast<T *>(::malloc(n * sizeof(T)));
            if (!p) {
                throw std::bad_alloc();
            }
            return p;
        }
        
        void deallocate(T * R2D9_NULLABLE p, std::size_t n) noexcept {
            if (p) {
                ::memset(p, 0, n * sizeof(T));
                ::free(p);
            }
        }
        
        bool operator == (const ZeroFillAllocator &) noexcept {
            return true;
        }
        
        bool operator != (const ZeroFillAllocator &) noexcept {
            return false;
        }
        
        ZeroFillAllocator() noexcept = default;
        ~ZeroFillAllocator() noexcept = default;
    };
    
    typedef std::array<uint8_t, 32> SHA256Buffer;
    typedef std::array<uint8_t, 64> HMAC512Buffer;
    typedef std::vector<uint8_t, ZeroFillAllocator<uint8_t> > ZeroFillDataVector;
    typedef std::basic_string<char, std::char_traits<char>, ZeroFillAllocator<char> > ZeroFillString;
    
    R2D9_CPP_API_PRIVATE(SHA256Buffer) sha256(const void * R2D9_NONNULL data, const size_t dataSize);
    
    R2D9_CPP_API_PRIVATE(HMAC512Buffer) hmacSha512(const void * R2D9_NONNULL key, const size_t keySize,
                                                   const void * R2D9_NONNULL data, const size_t dataSize);
    
    R2D9_CPP_API_PRIVATE(ZeroFillString) base64Encode(const void * R2D9_NONNULL data, const size_t dataSize);
    
    R2D9_CPP_API_PRIVATE(ZeroFillDataVector) base64Decode(const void * R2D9_NONNULL data, const size_t dataSize);
    
} // namespace crypto
} // namespace r2d9

#endif //!__R2D9_CRYPTO_HPP__
