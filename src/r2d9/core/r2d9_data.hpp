/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __R2D9_DATA_HPP__
#define __R2D9_DATA_HPP__ 1

#include <memory>
#include <cstring>
#include <algorithm>

#include "r2d9_raw_heap_memory.hpp"

namespace r2d9 {
    
/*
    class IData {
    public:
        virtual void reset(const size_t, const bool) = 0;
        
        virtual size_t size() const noexcept = 0;
        virtual void setSize(const size_t) noexcept = 0;
        
        virtual const uint8_t * R2D9_NULLABLE data() const noexcept = 0;
        virtual uint8_t * R2D9_NULLABLE data() noexcept = 0;
        
        virtual size_t capacity() const noexcept = 0;
        
        virtual size_t offset() const noexcept = 0;
        virtual void setOffset(const size_t) noexcept = 0;
        virtual size_t addOffset(const size_t) noexcept = 0;
        
        virtual void append(const void * R2D9_NONNULL, const size_t) = 0;
        
        virtual ~IData() noexcept = default;
    };
*/
    
    class Data final {
    private:
        RawHeapMemory _memory;
        size_t _size = 0;
        size_t _offset = 0;
        
        Data & operator = (Data &&) = delete;
        Data & operator = (const Data &) = delete;
        Data(Data &&) = delete;
        Data(const Data &) = delete;
        
    public:
        void reset(const size_t capacity = 0, const bool force = false) {
            if (force || (capacity > _memory.size())) {
                _memory.resize(capacity);
            }
            _size = _offset = 0;
        }
        
        size_t size() const noexcept {
            return _size;
        }
        
        void setSize(const size_t size) noexcept {
            const size_t capacity = _memory.size();
            _size = (size < capacity) ? size : capacity;
        }
        
        const void * R2D9_NULLABLE data() const noexcept {
            return _memory._memory;
        }
        
        void * R2D9_NULLABLE data() noexcept {
            return _memory._memory;
        }
        
        size_t capacity() const noexcept {
            return _memory.size();
        }
        
        size_t offset() const noexcept {
            return _offset;
        }
        
        void setOffset(const size_t offset) noexcept {
            _offset = (offset > _size) ? _size : offset;
        }
        
        size_t addOffset(const size_t offset) noexcept {
            const size_t finalOffset = offset + _offset;
            _offset = (finalOffset > _size) ? _size : finalOffset;
            return _offset;
        }
        
        void append(const void * R2D9_NONNULL data, const size_t dataSize) {
            if (dataSize) {
                DEBUG_ASSERT(data)
                const size_t capacity = _memory.size();
                const size_t availableSize = (_offset < capacity) ? (capacity - _offset) : 0;
                if (availableSize < dataSize) {
                    const size_t requiredExtraSize = dataSize - availableSize;
                    _memory.resize(capacity + requiredExtraSize);
                }
                ::memcpy(static_cast<uint8_t *>(_memory) + _offset, data, dataSize);
                _offset += dataSize;
                if (_offset > _size) {
                    _size = _offset;
                }
            }
        }
        
        Data() noexcept = default;
        ~Data() noexcept = default;
    };
    
} // namespace r2d9

#endif //!__R2D9_DATA_HPP__
