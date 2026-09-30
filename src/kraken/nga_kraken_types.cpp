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

    AssetPairStatus AssetPairStatusFromKey(const char * NGA_NULLABLE key) noexcept {
        if (key) {
            if (::strcasecmp(key, "online") == 0) {
                return AssetPairStatus::online;
            } else if (::strcasecmp(key, "cancel_only") == 0) {
                return AssetPairStatus::cancelOnly;
            } else if (::strcasecmp(key, "post_only") == 0) {
                return AssetPairStatus::postOnly;
            } else if (::strcasecmp(key, "limit_only") == 0) {
                return AssetPairStatus::limitOnly;
            } else if (::strcasecmp(key, "reduce_only") == 0) {
                return AssetPairStatus::reduceOnly;
            }
        }
        return AssetPairStatus{0};
    }
    
    const char * NGA_NONNULL AssetPairStatusToKey(const AssetPairStatus status) noexcept {
        switch (status) {
            case AssetPairStatus::online:     return "online";
            case AssetPairStatus::cancelOnly: return "cancel_only";
            case AssetPairStatus::postOnly:   return "post_only";
            case AssetPairStatus::limitOnly:  return "limit_only";
            case AssetPairStatus::reduceOnly: return "reduce_only";
            default: break;
        }
        return emptyCString;
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
