/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_TYPES_HPP__
#define __NGA_KRAKEN_TYPES_HPP__ 1

#include <limits>

#include "r2d9/r2d9.hpp"
#include "nga_types.hpp"

namespace nga {
namespace kraken {
    
    enum class OHLCPair : uint8_t {
        USDC_EUR    = 1,
        ATOM_USDC,
        BTC_USDC,
        ETH_USDC,
        POL_USDC,
        SOL_USDC,
        TON_USDC
    };
    
    R2D9_CPP_API(const char * R2D9_NONNULL) OHLCPairToKey(const OHLCPair pair) noexcept;
    R2D9_CPP_API(OHLCPair) OHLCPairFromKey(const char * R2D9_NULLABLE key) noexcept;
    R2D9_CPP_API(const char * R2D9_NONNULL) OHLCPairDBName(const OHLCPair pair) noexcept;
    
    R2D9_CPP_API(OrderStatus) OrderStatusFromKey(const char * R2D9_NULLABLE key) noexcept;
    R2D9_CPP_API(OrderType) OrderTypeFromKey(const char * R2D9_NULLABLE key) noexcept;
    
    R2D9_CPP_API(const char * R2D9_NONNULL const) orderTypeBuy;
    R2D9_CPP_API(const char * R2D9_NONNULL const) orderTypeSell;
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_TYPES_HPP__
