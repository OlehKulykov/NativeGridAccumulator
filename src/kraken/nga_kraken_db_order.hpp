/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_DB_ORDER_HPP__
#define __NGA_KRAKEN_DB_ORDER_HPP__ 1

#include <ctime>

#include "nga_kraken_order.hpp"

namespace nga {
namespace kraken {
    
    struct NGA_CPP_CLASS_API DBOrder final : public Order {
        int64_t id{-1};
        int64_t parentId{-1};
        int64_t createTimestamp{-1};
        int64_t updateTimestamp{-1};
        
        DBOrder & operator = (const DBOrder &) = delete;
        DBOrder(const DBOrder &) = delete;
        
        DBOrder & operator = (Order && order) noexcept {
            Order::operator=(static_cast<Order &&>(order));
            return *this;
        }
        
        DBOrder(Order && order) noexcept
            : Order(static_cast<Order &&>(order)) {
        }
        
        DBOrder & operator = (DBOrder &&) noexcept = default;
        DBOrder(DBOrder &&) noexcept = default;
        DBOrder() noexcept = default;
    };

} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_DB_ORDER_HPP__
