/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __R2D9_RAW_HEAP_MEMORY_HPP__
#define __R2D9_RAW_HEAP_MEMORY_HPP__ 1

#include <stdexcept>

#include "r2d9_pair.hpp"

namespace r2d9 {

    struct RawHeapMemory final {
    private:
        friend class Data;
#if defined(DEBUG)
        union {
            void * R2D9_NULLABLE _memory = nullptr;
            char * R2D9_NULLABLE _cstring;
            unsigned char * R2D9_NULLABLE _buffer;
            wchar_t * R2D9_NULLABLE _wstring;
        };
#else
        void * R2D9_NULLABLE _memory = nullptr;
#endif
        size_t _size = 0;
        
        RawHeapMemory & operator = (const RawHeapMemory &) = delete;
        RawHeapMemory(const RawHeapMemory &) = delete;
        
    public:
        inline size_t size() const noexcept { return _size; }
        
        RawHeapMemory & operator = (RawHeapMemory && memory) noexcept {
            if (_memory) {
                ::free(_memory);
            }
            _memory = memory._memory;
            _size = memory._size;
            memory._memory = nullptr;
            memory._size = 0;
            return *this;
        }
        
        inline operator bool () const noexcept { return (_memory != nullptr); }
        
        template<typename T>
        inline operator T () noexcept { return static_cast<T>(_memory); }
        
        template<typename T>
        inline operator T () const noexcept { return static_cast<T>(_memory); }
        
        PairPOD<void * R2D9_NULLABLE, size_t> take() noexcept {
            PairPOD<void * R2D9_NULLABLE, size_t> memPair;
            memPair.first = _memory;
            memPair.second = _size;
            _memory = nullptr;
            _size = 0;
            return memPair;
        }
        
        void reset(void * R2D9_NULLABLE memory = nullptr, const size_t size = 0) noexcept {
            if (_memory) {
                ::free(_memory);
            }
            _memory = memory;
            _size = size;
        }
        
        void resize(const size_t size) {
            if (size > 0) {
                void * memory = _memory ? ::realloc(_memory, size) : ::malloc(size);
                if (memory) {
                    _memory = memory;
                    _size = size;
                } else {
                    throw std::bad_alloc();
                }
            } else {
                if (_memory) {
                    ::free(_memory);
                    _memory = nullptr;
                }
                _size = 0;
            }
        }
        
        RawHeapMemory(const size_t size) {
            if (size) {
                if ( !(_memory = ::malloc(size)) ) {
                    throw std::bad_alloc();
                }
                _size = size;
            }
        }
        
        RawHeapMemory(RawHeapMemory && memory) noexcept :
            _memory(memory._memory),
            _size(memory._size) {
                memory._memory = nullptr;
                memory._size = 0;
        }
        
        RawHeapMemory() noexcept = default;
        
        ~RawHeapMemory() noexcept {
            if (_memory) {
                ::free(_memory);
            }
        }
    };

} // namespace r2d9

#endif //!__R2D9_RAW_HEAP_MEMORY_HPP__
