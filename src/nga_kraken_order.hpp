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

#include <string>
#include <sstream>

#include "r2d9/core/r2d9_crypto.hpp"

#include <boost/decimal.hpp>

#include "nga_kraken_types.hpp"

namespace nga {
namespace kraken {
    
    using boost::decimal::decimal64_t;
    using r2d9::crypto::ZeroFillString;
    
    struct R2D9_CPP_CLASS_API Order {
        ZeroFillString txId;
        ZeroFillString clientId;
        decimal64_t volume{-1.0};
        decimal64_t cost{-1.0};
        decimal64_t fee{-1.0};
        decimal64_t price{-1.0};
        decimal64_t openTimestamp{-1.0};
        OrderStatus status{0};
        OrderType type{0};
        OHLCPair pair{0};
        
        inline const ZeroFillString & clientOrTxId() const noexcept { return clientId.empty() ? txId : clientId; }
        
        Order & operator = (const Order &) = delete;
        Order(const Order &) = delete;
        
        Order & operator = (Order &&) noexcept = default;
        Order(Order &&) noexcept = default;
        Order() noexcept = default;
        
        template<typename T>
        static std::stringstream description(const T & order);
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_ORDER_HPP__
