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

#include "../core/nga_crypto.hpp"
#include "../core/nga_pair.hpp"
#include "../core/nga_trio.hpp"
#include "../nga_types.hpp"

namespace nga {
namespace kraken {
    
    enum class Token : uint8_t {
        USDC    = 1,
        ADA,
        ATOM,
        AVAX,
        BNB,
        BONK,
        BTC,
        DASH,
        ETH,
        EUR,
        FLOKI,
        LTC,
        PEPE,
        POL,
        SOL,
        TON,
        TRX,
        USD,
        USDT
    };
    
    NGA_CPP_API(const char * NGA_NONNULL) TokenToKey(const Token token) noexcept;
    
    enum class OHLCPair : uint8_t {
        USDC_EUR    = 1,
        ADA_USDC,
        ATOM_USDC,
        AVAX_USDC,
        BNB_USDC,
        BONK_USD,
        BTC_USDC,
        DASH_USD,
        ETH_USDC,
        FLOKI_USD,
        LTC_USDC,
        PEPE_USD,
        POL_USD,
        SOL_USDC,
        TON_USDC,
        TRX_USD
    };
    
    typedef PairPOD<Token, Token> OHLCPairTokens;
    
    NGA_CPP_API(OHLCPairTokens) OHLCPairToTokens(const OHLCPair pair) noexcept;
    NGA_CPP_API(const char * NGA_NONNULL) OHLCPairToKey(const OHLCPair pair) noexcept;
    NGA_CPP_API(OHLCPair) OHLCPairFromKey(const char * NGA_NULLABLE key) noexcept;
    
    NGA_CPP_API(OrderStatus) OrderStatusFromKey(const char * NGA_NULLABLE key) noexcept;
    NGA_CPP_API(const char * NGA_NONNULL) OrderStatusToKey(const OrderStatus status) noexcept;
    
    NGA_CPP_API(OrderType) OrderTypeFromKey(const char * NGA_NULLABLE key) noexcept;
    NGA_CPP_API(const char * NGA_NONNULL) OrderTypeToKey(const OrderType type) noexcept;
    
    NGA_CPP_API(const char * NGA_NONNULL const) orderTypeBuy;
    NGA_CPP_API(const char * NGA_NONNULL const) orderTypeSell;
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_TYPES_HPP__
