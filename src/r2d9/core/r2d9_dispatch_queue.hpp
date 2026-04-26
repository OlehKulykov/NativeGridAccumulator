/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __R2D9_DISPATCH_QUEUE_HPP__
#define __R2D9_DISPATCH_QUEUE_HPP__ 1

#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>

#include "r2d9_ring_queue.hpp"

namespace r2d9 {
    
    typedef std::function<void(void)> DispatchTask; // void(size_t)
    
    class DispatchQueue final {
    private:
        std::condition_variable _conditionVariable;
        std::mutex _mutex;
        std::vector<std::thread> _threads;
        RingQueue<DispatchTask> _tasks;
        bool _working = true;
        
        void work();
        
        static void join(std::thread * R2D9_NONNULL threads, const size_t count);
        
    public:
        DispatchQueue(const size_t threadsCount = 0);
        
        void add(DispatchTask && task);
        void clear();
        ~DispatchQueue();
    };
    
} // namespace r2d9

#endif //!__R2D9_DISPATCH_QUEUE_HPP__
