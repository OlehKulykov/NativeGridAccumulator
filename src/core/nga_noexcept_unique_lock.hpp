/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_NOEXCEPT_UNIQUE_LOCK_HPP__
#define __NGA_NOEXCEPT_UNIQUE_LOCK_HPP__ 1

#include <mutex>

#include "../nga.hpp"

namespace nga {
    
    template <class MUTEX>
    class NoExceptUniqueLock final {
    private:
        std::unique_lock<MUTEX> _lock;
        bool _locked;
        
    public:
        inline bool locked() const noexcept { return _locked; }
        
        void lock() noexcept {
            if (!_locked) {
                try {
                    _lock.lock();
                    _locked = true;
                } catch (...) {
                    /* do nothing */
                }
            }
        }
        
        void unlock() noexcept {
            if (_locked) {
                try {
                    _lock.unlock();
                    _locked = false;
                } catch (...) {
                    /* do nothing */
                }
            }
        }
        
        NoExceptUniqueLock & operator = (NoExceptUniqueLock &&) = delete;
        NoExceptUniqueLock & operator = (const NoExceptUniqueLock &) = delete;
        NoExceptUniqueLock(NoExceptUniqueLock &&) = delete;
        NoExceptUniqueLock(const NoExceptUniqueLock &) = delete;
        NoExceptUniqueLock() = delete;
        
        NoExceptUniqueLock(MUTEX & mutex) noexcept : _lock(mutex, std::defer_lock), _locked(false) {
            // 4) Does not lock the associated mutex.
            lock();
        }
        
        ~NoExceptUniqueLock() noexcept {
            unlock();
        }
    };
    
} // namespace nga

#endif //!__NGA_NOEXCEPT_UNIQUE_LOCK_HPP__
