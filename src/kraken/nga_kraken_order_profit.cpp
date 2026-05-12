/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <cstddef>

#include "nga_kraken_decimal_utils.hpp"

#include "nga_kraken_order_profit.hpp"

namespace nga {
namespace kraken {
    
    crypto::ZeroFillString OrderProfit::description() const {
        char tmpStr[maxDecimalCStringLen];
        crypto::ZeroFillStringStream stream;
        
        stream << OrderTypeToKey(from) << " > " << OrderTypeToKey(to);
        
        decimalToCString(volume, tmpStr);
        stream << ", volume: " << tmpStr;
        
        decimalToCString(cost, tmpStr);
        stream << ", cost: " << tmpStr;
        
        return stream.str();
    }
    
} // namespace kraken
} // namespace nga
