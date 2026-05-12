/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_CRYPTO_HPP__
#define __NGA_CRYPTO_HPP__ 1

#include <memory>
#include <array>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstring>

#include "nga_fixed_string_stream.hpp"

namespace nga {
namespace crypto {
    
    template<class T>
    inline void zeroFill(T & container, const size_t elementSize = 1) noexcept {
        auto * containerData = container.data();
        if (containerData) {
            ::memset(containerData, 0, container.size() * elementSize);
        }
    }
    
    template<typename T>
    class ZeroFillAllocator { // No 'final' - gcc bug
    public:
        using value_type = T;
        
        T * NGA_NONNULL allocate(std::size_t n) {
            T * p = static_cast<T *>(::malloc(n * sizeof(T)));
            if (!p) {
                throw std::bad_alloc();
            }
            return p;
        }
        
        void deallocate(T * NGA_NULLABLE p, std::size_t n) noexcept {
            if (p) {
                ::memset(p, 0, n * sizeof(T));
                ::free(p);
            }
        }
        
        bool operator == (const ZeroFillAllocator &) const noexcept {
            return true;
        }
        
        bool operator != (const ZeroFillAllocator &) const noexcept {
            return false;
        }
        
        ZeroFillAllocator() noexcept = default;
        ~ZeroFillAllocator() noexcept = default;
    };
    
    typedef std::array<uint8_t, 32> SHA256Buffer;
    typedef std::array<uint8_t, 64> HMAC512Buffer;
    typedef std::vector<uint8_t, ZeroFillAllocator<uint8_t> > ZeroFillDataVector;
    typedef std::basic_string<char, std::char_traits<char>, ZeroFillAllocator<char> > ZeroFillString;
    typedef std::basic_stringstream<char, std::char_traits<char>, ZeroFillAllocator<char> > ZeroFillStringStream;
    
    NGA_CPP_API_PRIVATE(SHA256Buffer) sha256(const void * NGA_NONNULL data, const size_t dataSize);
    
    NGA_CPP_API_PRIVATE(HMAC512Buffer) hmacSha512(const void * NGA_NONNULL key, const size_t keySize,
                                                   const void * NGA_NONNULL data, const size_t dataSize);
    
    NGA_CPP_API_PRIVATE(ZeroFillString) base64Encode(const void * NGA_NONNULL data, const size_t dataSize);
    
    NGA_CPP_API_PRIVATE(ZeroFillDataVector) base64Decode(const void * NGA_NONNULL data, const size_t dataSize);
    
} // namespace crypto
} // namespace nga

#endif //!__NGA_CRYPTO_HPP__
