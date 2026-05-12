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
#include <map>
#include <filesystem>

#include "../core/nga_crypto.hpp"
#include "../nga_types.hpp"

#include <boost/decimal/decimal128_t.hpp>

namespace nga {
namespace kraken {
    
    // OHLC market data
    struct OHLC final {
        double close;
        int64_t timestamp;
    };
    
    inline struct OHLC OHLCMake(const double close, const int64_t timestamp) noexcept {
        OHLC res;
        res.close = close;
        res.timestamp = timestamp;
        return res;
    }
    
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
    
    NGA_CPP_API(const char * NGA_NONNULL) OHLCPairToKey(const OHLCPair pair) noexcept;
    NGA_CPP_API(OHLCPair) OHLCPairFromKey(const char * NGA_NULLABLE key) noexcept;
    
    NGA_CPP_API(OrderStatus) OrderStatusFromKey(const char * NGA_NULLABLE key) noexcept;
    NGA_CPP_API(const char * NGA_NONNULL) OrderStatusToKey(const OrderStatus status) noexcept;
    
    NGA_CPP_API(OrderType) OrderTypeFromKey(const char * NGA_NULLABLE key) noexcept;
    NGA_CPP_API(const char * NGA_NONNULL) OrderTypeToKey(const OrderType type) noexcept;
    
    NGA_CPP_API(const char * NGA_NONNULL const) orderTypeBuy;
    NGA_CPP_API(const char * NGA_NONNULL const) orderTypeSell;
    
    typedef boost::decimal::decimal128_t decimal_t;
    
    struct OrderSettingsBase {
        decimal_t sellVolumeRate{-1};
        decimal_t sellCostRate{-1};
        decimal_t buyVolumeRate{-1};
        decimal_t buyCostRate{-1};
        decimal_t step{-1};
        uint32_t pairDecimals{0};
        uint32_t lotDecimals{0};
        
        OrderSettingsBase & operator = (const OrderSettingsBase &) = delete;
        OrderSettingsBase(const OrderSettingsBase &) = delete;
        
        OrderSettingsBase & operator = (OrderSettingsBase &&) noexcept = default;
        OrderSettingsBase(OrderSettingsBase &&) noexcept = default;
        OrderSettingsBase() noexcept = default;
    };
    
    struct Config final {
        crypto::ZeroFillString apiKey;
        crypto::ZeroFillString privateKey;
        std::map<OHLCPair, OrderSettingsBase> orderSettings;
        std::filesystem::path ordersBD;
        std::pair<uint32_t, uint32_t> checkOrdersTicks;
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_TYPES_HPP__
