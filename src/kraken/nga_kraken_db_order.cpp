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
    DBOrder Order::copy(const DBOrder & order) {
        // DBOrder res = Order::copy<Order>(order);
        DBOrder res;
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
        res.id = order.id;
        res.parentId = order.parentId;
        res.createTimestamp = order.createTimestamp;
        res.updateTimestamp = order.updateTimestamp;
        return res;
    }
    
    template<>
    crypto::ZeroFillStringStream Order::description(const DBOrder & order) {
        auto stream = Order::description<Order>(order);
        stream << ", id: " << order.id
        << ", parentId: " << order.parentId
        << ", createTs: " << order.createTimestamp
        << ", updateTs: " << order.updateTimestamp;
        return stream;
    }

} // namespace kraken
} // namespace nga
