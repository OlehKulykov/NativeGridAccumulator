/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_TYPES_HPP__
#define __NGA_TYPES_HPP__ 1

#include <utility>
#include <ctime>

#include "core/nga_trio.hpp"
#include "core/nga_crypto.hpp"

#include <boost/decimal/decimal128_t.hpp>

namespace nga {
    
    typedef crypto::ZeroFillDataVector      DataVector;
    typedef crypto::ZeroFillString          String;
    typedef crypto::ZeroFillStringStream    StringStream;
    typedef boost::decimal::decimal128_t    Decimal;
    typedef std::pair<Decimal, Decimal>     AskBid;
    typedef Trio<Decimal, Decimal, Decimal> AskBidIndex;
    
    enum class OrderStatus : uint8_t {
        open        = 1,
        closed      = 2,
        canceled    = 3,
        expired     = 4,
        pending     = 5
    };
    
    enum class OrderType : uint8_t {
        buy     = 1,
        sell    = 2
    };
    
} // namespace nga

#endif //!__NGA_TYPES_HPP__
