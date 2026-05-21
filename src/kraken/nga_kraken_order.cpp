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

#include "../core/nga_c_string.h"

#include "nga_kraken_decimal_utils.hpp"

#include "nga_kraken_order.hpp"

namespace nga {
namespace kraken {
 
    template<>
    Order Order::copy(const Order & order) {
        Order res;
        res.txId = order.txId;
        res.clientId = order.clientId;
        res.volume = order.volume;
        res.cost = order.cost;
        res.fee = order.fee;
        res.price = order.price;
        res.openTimestamp = order.openTimestamp;
        res.status = order.status;
        res.type = order.type;
        res.pair = order.pair;
        return res;
    }
    
    template<>
    StringStream Order::description(const Order & order) {
        char tmpStr[maxDecimalCStringLen];
        StringStream stream;
        
        stream << OHLCPairToKey(order.pair)
        << ", " << OrderTypeToKey(order.type);
        
        decimalToCString(order.volume, tmpStr);
        stream << ", volume: " << tmpStr;
        
        decimalToCString(order.price, tmpStr);
        stream << ", price: " << tmpStr;
        
        decimalToCString(order.cost, tmpStr);
        stream << ", cost: " << tmpStr;
        
        decimalToCString(order.fee, tmpStr);
        stream << ", fee: " << tmpStr;
        
#if defined(DEBUG)
        decimalToCString((order.cost + order.fee), tmpStr);
        stream << ", cost+fee: " << tmpStr;
#endif
        
        stream << ", status: " << OrderStatusToKey(order.status)
        << ", txId: " << (order.txId.c_str() ?: emptyCString)
        << ", clId: " << (order.clientId.c_str() ?: emptyCString);
        
        decimalToCString(order.openTimestamp, tmpStr);
        stream << ", opents: " << tmpStr;
        
        return stream;
    }
    
} // namespace kraken
} // namespace nga
