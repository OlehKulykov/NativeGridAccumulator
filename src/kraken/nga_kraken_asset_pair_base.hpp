/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_ASSET_PAIR_HPP__
#define __NGA_KRAKEN_ASSET_PAIR_HPP__ 1

#include "nga_kraken_types.hpp"

namespace nga {
namespace kraken {
    
    struct AssetPairBase {
        String name;
        String base;
        uint32_t pairDecimals{0};
        uint32_t lotDecimals{0};
        AssetPairStatus status{0};
    };
        
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_ASSET_PAIR_HPP__
