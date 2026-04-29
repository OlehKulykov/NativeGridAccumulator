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
    
} // namespace kraken
} // namespace nga
