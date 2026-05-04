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

#include "nga_kraken_db_order.hpp"

#include "r2d9/core/r2d9_sqlite_db.hpp"

namespace nga {
namespace kraken {
    
    class R2D9_CPP_CLASS_API OrdersDB final : public r2d9::SQLiteDB {
    public:
        OrdersDB & open(const char * R2D9_NONNULL path, const bool init = false);
        
        std::vector<DBOrder> selectFromTimestamp(const int64_t fromTimestamp);
        
        int64_t insert();
        
        int64_t insert(const DBOrder & order);
        
        void insertReplace(const DBOrder & order);
        
        OrdersDB(const char * R2D9_NONNULL path, const bool init = false);
        OrdersDB() noexcept = default;
        ~OrdersDB() noexcept = default;
    };
        
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_ORDERS_DB_HPP__
