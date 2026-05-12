/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_RING_QUEUE_HPP__
#define __NGA_RING_QUEUE_HPP__ 1

#include <utility>

#include "nga_pair.hpp"

namespace nga {
    
    template<class T, typename SIZE_TYPE = size_t, SIZE_TYPE CAPACITY_INC = 8>
    class RingQueue final {
    private:
        struct Node final {
            struct Node * next = nullptr;
            struct Node * prev = nullptr;
            T element;
        };
        
        struct NodeDeleter final {
            void operator()(struct Node * node) const noexcept {
                RingQueue::deleteNodes(node);
            }
        };
        
        struct Node * _front = nullptr;
        struct Node * _back = nullptr;
        SIZE_TYPE _size = 0;
        SIZE_TYPE _capacity = 0;
        
        void extend(const SIZE_TYPE capacity) {
            const auto nodes = createNodes(capacity);
            if (_back) {
                _back->next = nodes.first;
                _back->next->prev = _back;
                _front->prev = nodes.second;
                _front->prev->next = _front;
            } else {
                _front = nodes.first;
                _front->prev = nodes.second;
                _front->prev->next = _front;
            }
            _capacity += capacity;
        }
        
        static PairPOD<struct Node *, struct Node *> createNodes(const size_t capacity) {
            std::unique_ptr<struct Node, NodeDeleter> front(new struct Node(), NodeDeleter());
            struct Node * curr = front.get();
            for (size_t i = 1; i < capacity; i++) {
                struct Node * next = new struct Node();
                curr->next = next;
                next->prev = curr;
                curr = next;
            }
            PairPOD<struct Node *, struct Node *> pair;
            pair.first = front.release();
            pair.second = curr;
            return pair;
        }
        
        static void deleteNodes(struct Node * begin) noexcept {
            if (begin) {
                struct Node * curr = begin;
                do {
                    struct Node * next = curr->next;
                    delete curr;
                    curr = next;
                } while (curr && (curr != begin));
            }
        }
        
    public:
        inline size_t size() const noexcept { return _size; }
        
        inline size_t capacity() const noexcept { return _capacity; }
        
        T & front() noexcept {
            DEBUG_ASSERT(_front != nullptr)
            return _front->element;
        }
        
        const T & front() const noexcept {
            DEBUG_ASSERT(_front != nullptr)
            return _front->element;
        }
        
        T & back() noexcept {
            DEBUG_ASSERT(_back != nullptr)
            return _back->element;
        }
        
        const T & back() const noexcept {
            DEBUG_ASSERT(_back != nullptr)
            return _back->element;
        }
        
        void pushBack(const T & value) {
            if (_size == _capacity) {
                extend(CAPACITY_INC);
            }
            struct Node * nextBack = _back ? _back->next : _front;
            nextBack->element = value;
            _back = nextBack;
            _size++;
        }
        
        void pushBack(T && value) {
            if (_size == _capacity) {
                extend(CAPACITY_INC);
            }
            struct Node * nextBack = _back ? _back->next : _front;
            nextBack->element = static_cast<T &&>(value);
            _back = nextBack;
            _size++;
        }
        
        void pushFront(const T & value) {
            if (_size == _capacity) {
                extend(CAPACITY_INC);
            }
            struct Node * prevFront = _back ? _front->prev : _front;
            prevFront->element = value;
            if (!_back) {
                _back = _front;
            }
            _front = prevFront;
            _size++;
        }
        
        void pushFront(T && value) {
            if (_size == _capacity) {
                extend(CAPACITY_INC);
            }
            struct Node * prevFront = _back ? _front->prev : _front;
            prevFront->element = static_cast<T &&>(value);
            if (!_back) {
                _back = _front;
            }
            _front = prevFront;
            _size++;
        }
        
        T popBack() noexcept {
            DEBUG_ASSERT((_size > 0) && (_back != nullptr))
            auto element = static_cast<T &&>(_back->element);
            if (--_size > 0) {
                _back = _back->prev;
            } else {
                _back = nullptr;
            }
            return element;
        }
        
        T popFront() noexcept {
            DEBUG_ASSERT((_size > 0) && (_front != nullptr))
            auto element = static_cast<T &&>(_front->element);
            if (--_size > 0) {
                _front = _front->next;
            } else {
                _back = nullptr;
            }
            return element;
        }
        
        void clear() noexcept {
            struct Node * front = _front;
            _front = _back = nullptr;
            _size = _capacity = 0;
            deleteNodes(front);
        }
        
        void reserve(const SIZE_TYPE capacity) {
            if (capacity > _capacity) {
                extend(capacity - _capacity);
            }
        }
        
        RingQueue(RingQueue && deque) noexcept :
            _front(deque._front),
            _back(deque._back),
            _size(deque._size),
            _capacity(deque._capacity) {
                deque._front = deque._back = nullptr;
                deque._size = deque._capacity = 0;
        }
        
        RingQueue(const SIZE_TYPE capacity = 0) {
            if (capacity > 0) {
                extend(capacity);
            }
        }
        
        ~RingQueue() noexcept {
            deleteNodes(_front);
        }
        
#if defined(NGA_OUTPUT)
        void print() {
            std::flush(std::cout) << "RingQueue:[";
            struct Node * curr = _front;
            for (size_t i = 0; i < _size; i++) {
                std::cout << (i ? ", " : "") << curr->element;
                curr = curr->next;
            }
            std::cout << "]" << std::endl;
        }
#endif
    };
    
} // namespace nga

#endif //!__NGA_RING_QUEUE_HPP__
