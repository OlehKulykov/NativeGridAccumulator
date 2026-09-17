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
    
    enum AssetPairStatus : uint8_t {
        online      = 1,
        cancelOnly,
        postOnly,
        limitOnly,
        reduceOnly
    };
    
    NGA_CPP_API(AssetPairStatus) AssetPairStatusFromKey(const char * NGA_NULLABLE key) noexcept;
    NGA_CPP_API(const char * NGA_NONNULL) AssetPairStatusToKey(const AssetPairStatus status) noexcept;
    
    NGA_CPP_API(OrderStatus) OrderStatusFromKey(const char * NGA_NULLABLE key) noexcept;
    NGA_CPP_API(const char * NGA_NONNULL) OrderStatusToKey(const OrderStatus status) noexcept;
    
    NGA_CPP_API(OrderType) OrderTypeFromKey(const char * NGA_NULLABLE key) noexcept;
    NGA_CPP_API(const char * NGA_NONNULL) OrderTypeToKey(const OrderType type) noexcept;
    
    NGA_CPP_API(const char * NGA_NONNULL const) orderTypeBuy;
    NGA_CPP_API(const char * NGA_NONNULL const) orderTypeSell;
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_TYPES_HPP__
