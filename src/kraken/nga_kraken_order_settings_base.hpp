/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_ORDER_SETTINGS_BASE_HPP__
#define __NGA_KRAKEN_ORDER_SETTINGS_BASE_HPP__ 1

#include <cstddef>

#include "nga_kraken_types.hpp"

namespace nga {
namespace kraken {
    
    struct NGA_CPP_CLASS_API OrderSettingsBase {
        Decimal sellVolumeRate{-1};
        Decimal sellCostRate{-1};
        Decimal buyVolumeRate{-1};
        Decimal buyCostRate{-1};
        Decimal step{-1};
        uint32_t pairDecimals{0};
        uint32_t lotDecimals{0};
        bool enabled{false};
        
        OrderSettingsBase & operator = (const OrderSettingsBase &) = delete;
        OrderSettingsBase(const OrderSettingsBase &) = delete;
        
        OrderSettingsBase & operator = (OrderSettingsBase &&) noexcept = default;
        OrderSettingsBase(OrderSettingsBase &&) noexcept = default;
        OrderSettingsBase() noexcept = default;
    };
        
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_ORDER_SETTINGS_BASE_HPP__
