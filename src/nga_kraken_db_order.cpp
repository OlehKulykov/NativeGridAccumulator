/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <sstream>
#include <iomanip>

#include "nga_kraken_db_order.hpp"

namespace nga {
namespace kraken {
    
    template<>
    std::stringstream Order::description(const DBOrder & order) {
        auto stream = description<Order>(order);
        stream << ", id: " << order.id
        << ", parentId: " << order.parentId
        << ", timestamp: " << order.timestamp;
        return stream;
    }
    
    DBOrder DBOrder::copy() const {
        DBOrder order;
        order.txId = txId;
        order.clientId = clientId;
        order.volume = volume;
        order.cost = cost;
        order.fee = fee;
        order.price = price;
        order.openTimestamp = openTimestamp;
        order.status = status;
        order.type = type;
        order.pair = pair;
        order.id = id;
        order.parentId = parentId;
        order.timestamp = timestamp;
        return order;
    }
    
} // namespace kraken
} // namespace nga
