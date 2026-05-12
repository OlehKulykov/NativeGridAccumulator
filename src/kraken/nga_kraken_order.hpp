/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_ORDER_HPP__
#define __NGA_KRAKEN_ORDER_HPP__ 1

#include <ostream>
#include <sstream>

#include "../core/nga_crypto.hpp"

#include "nga_kraken_order_profit.hpp"
#include "nga_kraken_icalc.hpp"
#include "nga_kraken_types.hpp"

namespace nga {
namespace kraken {
    
    struct NGA_CPP_CLASS_API Order {
        crypto::ZeroFillString txId;
        crypto::ZeroFillString clientId;
        decimal_t volume{-1};
        decimal_t cost{-1};
        decimal_t fee{-1};
        decimal_t price{-1};
        decimal_t openTimestamp{-1}; // Unix timestamp of when order was placed
        OrderStatus status{0};
        OrderType type{0};
        OHLCPair pair{0};
        
        inline const crypto::ZeroFillString & clOrTxId() const noexcept { return clientId.empty() ? txId : clientId; }
        
        Order & operator = (const Order &) = delete;
        Order(const Order &) = delete;
        
        Order & operator = (Order &&) noexcept = default;
        Order(Order &&) noexcept = default;
        Order() noexcept = default;
        
        template<typename T>
        static crypto::ZeroFillStringStream description(const T & order);
        
        template<typename T>
        static T copy(const T & order);
        
        template<typename T>
        inline friend crypto::ZeroFillStringStream & operator << (crypto::ZeroFillStringStream & os, const T & order) {
            os << Order::description(order).str();
            return os;
        }
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_ORDER_HPP__
