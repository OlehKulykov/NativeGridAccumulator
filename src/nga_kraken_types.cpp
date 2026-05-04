/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <limits>
#include <cstring>

#include "nga_kraken_types.hpp"

#include "r2d9/core/r2d9_pair.hpp"
#include "r2d9/core/r2d9_c_string.h"

namespace nga {
namespace kraken {

    // https://api.kraken.com/0/public/AssetPairs
    //TODO: ::sort + ::bsearch
    
    typedef r2d9::PairPOD<const char *, OHLCPair> OHLCPairKeyType;
    
    static constexpr size_t _OHLCPairKeyTypeArraySize = 7;
    
    static OHLCPairKeyType _OHLCPairKeysToTypes[_OHLCPairKeyTypeArraySize] = {
        { "USDCEUR", OHLCPair::USDC_EUR },
        { "ATOMUSDC", OHLCPair::ATOM_USDC },
        { "BTCUSDC", OHLCPair::BTC_USDC },
        { "ETHUSDC", OHLCPair::ETH_USDC },
        { "POLUSDC", OHLCPair::POL_USDC },
        { "SOLUSDC", OHLCPair::SOL_USDC },
        { "TONUSDC", OHLCPair::TON_USDC }
    };
    
    const char * R2D9_NONNULL OHLCPairToKey(const OHLCPair pair) noexcept {
        const auto * keyTypes = static_cast<const OHLCPairKeyType *>(_OHLCPairKeysToTypes);
        for (size_t i = 0; i < _OHLCPairKeyTypeArraySize; i++, ++keyTypes) {
            if (keyTypes->second == pair) {
                return keyTypes->first;
            }
        }
        return emptyCString;
    }
    
    OHLCPair OHLCPairFromKey(const char * R2D9_NULLABLE key) noexcept {
        if (key) {
            const auto * keyTypes = static_cast<const OHLCPairKeyType *>(_OHLCPairKeysToTypes);
            for (size_t i = 0; i < _OHLCPairKeyTypeArraySize; i++, ++keyTypes) {
                if (::strcasecmp(key, keyTypes->first) == 0) {
                    return keyTypes->second;
                }
            }
        }
        return OHLCPair{0};
    }
    
    const char * R2D9_NONNULL OHLCPairDBName(const OHLCPair pair) noexcept {
        switch (pair) {
            case OHLCPair::USDC_EUR:  return "ohlc.usdc.eur.sqlite";
            case OHLCPair::ATOM_USDC: return "ohlc.atom.usdc.sqlite";
            case OHLCPair::BTC_USDC:  return "ohlc.btc.usdc.sqlite";
            case OHLCPair::ETH_USDC:  return "ohlc.eth.usdc.sqlite";
            case OHLCPair::POL_USDC:  return "ohlc.pol.usdc.sqlite";
            case OHLCPair::SOL_USDC:  return "ohlc.sol.usdc.sqlite";
            case OHLCPair::TON_USDC:  return "ohlc.ton.usdc.sqlite";
            default: break;
        }
        return emptyCString;
    }
    
    OrderStatus OrderStatusFromKey(const char * R2D9_NULLABLE key) noexcept {
        if (key) {
            if (::strcasecmp(key, "open") == 0) {
                return OrderStatus::open;
            } else if (::strcasecmp(key, "closed") == 0) {
                return OrderStatus::closed;
            } else if (::strcasecmp(key, "canceled") == 0) {
                return OrderStatus::canceled;
            } else if (::strcasecmp(key, "expired") == 0) {
                return OrderStatus::expired;
            } else if (::strcasecmp(key, "pending") == 0) {
                return OrderStatus::pending;
            }
        }
        return OrderStatus{0};
    }
    
    OrderType OrderTypeFromKey(const char * R2D9_NULLABLE key) noexcept {
        if (key) {
            if (::strcasecmp(key, orderTypeBuy) == 0) {
                return OrderType::buy;
            } else if (::strcasecmp(key, orderTypeSell) == 0) {
                return OrderType::sell;
            }
        }
        return OrderType{0};
    }
    
    const char * R2D9_NONNULL const orderTypeBuy = "buy";
    const char * R2D9_NONNULL const orderTypeSell = "sell";
    
} // namespace kraken
} // namespace nga
