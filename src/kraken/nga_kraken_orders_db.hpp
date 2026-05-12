/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_ORDERS_DB_HPP__
#define __NGA_KRAKEN_ORDERS_DB_HPP__ 1

#include <vector>

#include "../core/nga_sqlite_db.hpp"

#include "nga_kraken_db_order.hpp"

namespace nga {
namespace kraken {
    
    class NGA_CPP_CLASS_API OrdersDB final : public SQLiteDB {
    public:
        OrdersDB & open(const char * NGA_NONNULL path, const bool init = false);
        
        std::vector<DBOrder> selectFromCreateTimestamp(const int64_t fromTimestamp);
        
        DBOrder selectById(const int64_t orderId);
        
        int64_t insert();
        
        int64_t insert(const DBOrder & order);
        
        void insertReplace(const DBOrder & order);
        
        void insertReplace(const std::vector<DBOrder> & orders);
        
        OrdersDB(const char * NGA_NONNULL path, const bool init = false);
        OrdersDB() noexcept = default;
        ~OrdersDB() noexcept = default;
    };
        
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_ORDERS_DB_HPP__
