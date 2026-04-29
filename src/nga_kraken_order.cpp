/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <algorithm>
#include <sstream>
#include <iomanip>

#include "nga_kraken_decimal_utils.hpp"
#include "r2d9/core/r2d9_c_string.h"

#include "nga_kraken_order.hpp"

namespace nga {
namespace kraken {
 
    template<>
    std::stringstream Order::description(const Order & order) {
        char tmpStr[maxDecimalCStringLen];
        std::stringstream stream;
        
        stream << OHLCPairToKey(order.pair)
        << ", " << ((order.type == OrderType::buy) ? orderTypeBuy : orderTypeSell);
        
        decimalToCString(order.volume, tmpStr);
        stream << ", volume: " << tmpStr;
        
        decimalToCString(order.price, tmpStr);
        stream << ", price: " << tmpStr;
        
        decimalToCString(order.cost, tmpStr);
        stream << ", cost: " << tmpStr;
        
        decimalToCString(order.fee, tmpStr);
        stream << ", fee: " << tmpStr;
        
        stream << ", txId: " << (order.txId.c_str() ?: emptyCString)
        << ", clId: " << (order.clientId.c_str() ?: emptyCString);
        
        decimalToCString(order.openTimestamp, tmpStr);
        stream << ", opents: " << tmpStr;
        
        return stream;
    }
    
} // namespace kraken
} // namespace nga
