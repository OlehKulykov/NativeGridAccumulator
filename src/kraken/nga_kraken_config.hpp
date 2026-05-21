/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_CONFIG_HPP__
#define __NGA_KRAKEN_CONFIG_HPP__ 1

#include <map>
#include <utility>
#include <filesystem>

#include "nga_kraken_types.hpp"
#include "nga_kraken_order_settings_base.hpp"

namespace nga {
namespace kraken {
    
    struct NGA_CPP_CLASS_API Config final {
        String apiKey;
        String privateKey;
        std::map<OHLCPair, OrderSettingsBase> orderSettings;
        std::filesystem::path ordersBD;
        std::pair<uint32_t, uint32_t> checkOrdersTicks;
    };
        
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_CONFIG_HPP__
