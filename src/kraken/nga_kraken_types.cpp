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

#include "../core/nga_pair.hpp"
#include "../core/nga_c_string.h"

#include "nga_kraken_types.hpp"

namespace nga {
namespace kraken {

    const char * NGA_NONNULL TokenToKey(const Token token) noexcept {
        switch (token) {
            case Token::USDC:  return "USDC";
            case Token::ADA:   return "ADA";
            case Token::ATOM:  return "ATOM";
            case Token::AVAX:  return "AVAX";
            case Token::BNB:   return "BNB";
            case Token::BONK:  return "BONK";
            case Token::BTC:   return "BTC";
            case Token::DASH:  return "DASH";
            case Token::ETH:   return "ETH";
            case Token::EUR:   return "EUR";
            case Token::FLOKI: return "FLOKI";
            case Token::LTC:   return "LTC";
            case Token::PEPE:  return "PEPE";
            case Token::POL:   return "POL";
            case Token::SOL:   return "SOL";
            case Token::TON:   return "TON";
            case Token::TRX:   return "TRX";
            case Token::USD:   return "USD";
            case Token::USDT:  return "USDT";
            default: break;
        }
        return emptyCString;
    }
    
    // https://api.kraken.com/0/public/AssetPairs
    //TODO: ::sort + ::bsearch
    
    OHLCPairTokens OHLCPairToTokens(const OHLCPair pair) noexcept {
        OHLCPairTokens res;
        switch (pair) {
            case OHLCPair::USDC_EUR:  res.first = Token::USDC;  res.second = Token::EUR;  break;
            case OHLCPair::ADA_USDC:  res.first = Token::ADA;   res.second = Token::USDC; break;
            case OHLCPair::ATOM_USDC: res.first = Token::ATOM;  res.second = Token::USDC; break;
            case OHLCPair::AVAX_USDC: res.first = Token::AVAX;  res.second = Token::USDC; break;
            case OHLCPair::BNB_USDC:  res.first = Token::BNB;   res.second = Token::USDC; break;
            case OHLCPair::BONK_USD:  res.first = Token::BONK;  res.second = Token::USD;  break;
            case OHLCPair::BTC_USDC:  res.first = Token::BTC;   res.second = Token::USDC; break;
            case OHLCPair::DASH_USD:  res.first = Token::DASH;  res.second = Token::USD;  break;
            case OHLCPair::ETH_USDC:  res.first = Token::ETH;   res.second = Token::USDC; break;
            case OHLCPair::FLOKI_USD: res.first = Token::FLOKI; res.second = Token::USD;  break;
            case OHLCPair::LTC_USDC:  res.first = Token::LTC;   res.second = Token::USDC; break;
            case OHLCPair::PEPE_USD:  res.first = Token::PEPE;  res.second = Token::USD;  break;
            case OHLCPair::POL_USD:   res.first = Token::POL;   res.second = Token::USD;  break;
            case OHLCPair::SOL_USDC:  res.first = Token::SOL;   res.second = Token::USDC; break;
            case OHLCPair::TON_USDC:  res.first = Token::TON;   res.second = Token::USDC; break;
            case OHLCPair::TRX_USD:   res.first = Token::TRX;   res.second = Token::USD;  break;
            default: res.first = Token{0}; res.second = Token{0}; break;
        }
        return res;
    }
    
    typedef PairPOD<const char *, OHLCPair> OHLCPairKeyType;
    
    constexpr const size_t _OHLCPairKeyTypeArraySize = 16;
    
    static OHLCPairKeyType _OHLCPairKeysToTypes[_OHLCPairKeyTypeArraySize] = {
        { "USDCEUR",  OHLCPair::USDC_EUR },
        { "ADAUSDC",  OHLCPair::ADA_USDC },
        { "ATOMUSDC", OHLCPair::ATOM_USDC },
        { "AVAXUSDC", OHLCPair::AVAX_USDC },
        { "BNBUSDC",  OHLCPair::BNB_USDC },
        { "BONKUSD",  OHLCPair::BONK_USD },
        { "BTCUSDC",  OHLCPair::BTC_USDC },
        { "DASHUSD",  OHLCPair::DASH_USD },
        { "ETHUSDC",  OHLCPair::ETH_USDC },
        { "FLOKIUSD", OHLCPair::FLOKI_USD },
        { "LTCUSDC",  OHLCPair::LTC_USDC },
        { "PEPEUSD",  OHLCPair::PEPE_USD },
        { "POLUSD",   OHLCPair::POL_USD },
        { "SOLUSDC",  OHLCPair::SOL_USDC },
        { "TONUSDC",  OHLCPair::TON_USDC },
        { "TRXUSD",   OHLCPair::TRX_USD }
    };
    
    const char * NGA_NONNULL OHLCPairToKey(const OHLCPair pair) noexcept {
        const auto * keyTypes = static_cast<const OHLCPairKeyType *>(_OHLCPairKeysToTypes);
        for (size_t i = 0; i < _OHLCPairKeyTypeArraySize; i++, ++keyTypes) {
            if (keyTypes->second == pair) {
                return keyTypes->first;
            }
        }
        return emptyCString;
    }
    
    OHLCPair OHLCPairFromKey(const char * NGA_NULLABLE key) noexcept {
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
    
    OrderStatus OrderStatusFromKey(const char * NGA_NULLABLE key) noexcept {
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
    
    const char * NGA_NONNULL OrderStatusToKey(const OrderStatus status) noexcept {
        switch (status) {
            case OrderStatus::open:     return "open";
            case OrderStatus::closed:   return "closed";
            case OrderStatus::canceled: return "canceled";
            case OrderStatus::expired:  return "expired";
            case OrderStatus::pending:  return "pending";
            default: break;
        }
        return emptyCString;
    }
    
    OrderType OrderTypeFromKey(const char * NGA_NULLABLE key) noexcept {
        if (key) {
            if (::strcasecmp(key, orderTypeBuy) == 0) {
                return OrderType::buy;
            } else if (::strcasecmp(key, orderTypeSell) == 0) {
                return OrderType::sell;
            }
        }
        return OrderType{0};
    }
    
    const char * NGA_NONNULL OrderTypeToKey(const OrderType type) noexcept {
        switch (type) {
            case OrderType::buy:  return orderTypeBuy;
            case OrderType::sell: return orderTypeSell;
            default: break;
        }
        return emptyCString;
    }
    
    const char * NGA_NONNULL const orderTypeBuy = "buy";
    const char * NGA_NONNULL const orderTypeSell = "sell";
    
} // namespace kraken
} // namespace nga
