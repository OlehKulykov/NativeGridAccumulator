/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <memory>

#define __NGA_FILE__ "DispatchQueue"

#include "nga_dispatch_queue.hpp"

namespace nga {
    
    void DispatchQueue::work() {
#if defined(DEBUG) && defined(__APPLE__)
        char threadName[32];
        ::snprintf(threadName, 32, "Dispatch thread");
        pthread_setname_np(threadName);
#endif
        std::unique_lock<std::mutex> lock(_mutex);
        do {
            _conditionVariable.wait(lock, [this] {
                return (_tasks.size() || !_working);
            });
            
            if (_tasks.size() && _working) {
                auto task = _tasks.popFront();
                lock.unlock();
                
                task();
                
                lock.lock();
            }
        } while (_working);
    }
    
    void DispatchQueue::add(DispatchTask && task) {
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.pushBack(static_cast<DispatchTask &&>(task));
        lock.unlock();
        _conditionVariable.notify_one();
    }
    
    void DispatchQueue::clear() {
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.clear();
        _tasks.reserve(_threads.size() * 2);
        lock.unlock();
        _conditionVariable.notify_all();
    }
    
    DispatchQueue::DispatchQueue(const size_t threadsCount) {
        const size_t count = ((threadsCount > 0) && (threadsCount < 256)) ? threadsCount : 1;
        _threads.reserve(count);
        try {
            for (size_t i = 0; i < count; i++) {
                _threads.emplace_back(std::thread(&DispatchQueue::work, this));
            }
        } catch (...) {
            _working = false;
            _conditionVariable.notify_all();
            DispatchQueue::join(_threads.data(), _threads.size());
            throw;
        }
    }
    
    DispatchQueue::~DispatchQueue() {
        std::unique_lock<std::mutex> lock(_mutex);
        _working = false;
        _tasks.clear();
        lock.unlock();
        _conditionVariable.notify_all();
        DispatchQueue::join(_threads.data(), _threads.size());
    }
    
    void DispatchQueue::join(std::thread * NGA_NONNULL threads, const size_t count) {
        std::thread * thread = threads;
        for (size_t i = 0; i < count; i++, thread++) {
            thread->join();
        }
    }
    
} // namespace nga
