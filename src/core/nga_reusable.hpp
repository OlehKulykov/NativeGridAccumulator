/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_REUSABLE_HPP__
#define __NGA_REUSABLE_HPP__ 1

#include <memory>
#include <mutex>

#include "nga_ring_queue.hpp"
#include "nga_pair.hpp"

namespace nga {
    
    template<class T>
    class Reusable : public std::enable_shared_from_this<Reusable<T> > {
    private:
        struct ElementDeleter final {
            std::weak_ptr<Reusable<T> > parent;
            
            void operator()(T * element) const noexcept {
                std::unique_ptr<T> elem(element);
                const auto prnt = parent.lock();
                if (prnt) {
                    try {
                        prnt->reuse(static_cast<std::unique_ptr<T> &&>(elem));
                    } catch (...) {
                        // do nothing, if not reused/moved, then delete as non-null std::unique_ptr<T> elem.
                    }
                }
            }
        };
        
        RingQueue<std::unique_ptr<T>, uint32_t, 4> _queue;
        
    protected:
        virtual void reuse(std::unique_ptr<T> && element) {
            _queue.pushBack(static_cast<std::unique_ptr<T> &&>(element));
        }
        
    public:
        virtual PairPOD<size_t, size_t> sizecap() const {
            PairPOD<size_t, size_t> res;
            res.first = _queue.size();
            res.second = _queue.capacity();
            return res;
        }
        
        virtual std::shared_ptr<T> get() {
            auto element = _queue.size() ? _queue.popFront() : std::make_unique<T>();
            struct ElementDeleter deleter;
            deleter.parent = this->weak_from_this();
            return std::shared_ptr<T>(element.release(), static_cast<ElementDeleter &&>(deleter));
        }
        
        virtual std::pair<std::shared_ptr<T>, std::shared_ptr<T> > pair() {
            return std::pair<std::shared_ptr<T>, std::shared_ptr<T> >( Reusable<T>::get(), Reusable<T>::get() );
        }
        
        Reusable(const size_t capacity = 0) : std::enable_shared_from_this<Reusable<T> >(), _queue(static_cast<uint32_t>(capacity)) { }
        virtual ~Reusable() noexcept = default;
    };
    
    template<class T>
    class ReusableMT final : public Reusable<T> {
    private:
        mutable std::mutex _mutex;
        
    protected:
        virtual void reuse(std::unique_ptr<T> && element) override final {
            const std::lock_guard<std::mutex> lock(_mutex);
            Reusable<T>::reuse(static_cast<std::unique_ptr<T> &&>(element));
        }
        
    public:
        virtual PairPOD<size_t, size_t> sizecap() const override final {
            const std::lock_guard<std::mutex> lock(_mutex);
            return Reusable<T>::sizecap();
        }
        
        virtual std::shared_ptr<T> get() override final {
            const std::lock_guard<std::mutex> lock(_mutex);
            return Reusable<T>::get();
        }
        
        virtual std::pair<std::shared_ptr<T>, std::shared_ptr<T> > pair() override final {
            const std::lock_guard<std::mutex> lock(_mutex);
            return Reusable<T>::pair();
        }
        
        ReusableMT(const size_t capacity = 0) : Reusable<T>(capacity) { }
        virtual ~ReusableMT() noexcept = default;
    };
    
} // namespace nga

#endif //!__NGA_REUSABLE_HPP__
