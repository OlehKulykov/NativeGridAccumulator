/*
 * Copyright (C) Oleh Kulykov <olehkulykov@gmail.com>. All Rights Reserved.
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Created by Oleh Kulykov <olehkulykov@gmail.com>, 2018-2025.
 */

#ifndef __NGA_SCOPE_GUARD_HPP__
#define __NGA_SCOPE_GUARD_HPP__ 1

#include <functional>

#include "nga_pair.hpp"

namespace nga {
    
    class ScopeGuard final {
    private:
        std::function<void()> _defer;
        
        ScopeGuard & operator = (ScopeGuard &&) = delete;
        ScopeGuard & operator = (const ScopeGuard &) = delete;
        ScopeGuard(ScopeGuard &&) = delete;
        ScopeGuard(const ScopeGuard &) = delete;
        ScopeGuard() = delete;
        
    public:
        void cancel() noexcept {
            _defer = nullptr;
        }
        
        ScopeGuard(std::function<void()> && defer) noexcept :
            _defer(static_cast<std::function<void()> &&>(defer)) {
            
        }
        
        ~ScopeGuard() noexcept {
            if (_defer) {
                _defer();
            }
        }
    };
    
} // namespace nga

#endif //!__NGA_SCOPE_GUARD_HPP__
