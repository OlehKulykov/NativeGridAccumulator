/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_SQLITE_DB_HPP__
#define __NGA_SQLITE_DB_HPP__ 1

#include <memory>

#include "../nga.hpp"

struct sqlite3;
struct sqlite3_stmt;

namespace nga {
    
    class SQLiteDB {
    protected:
        struct DBDeleter final {
            void operator()(struct sqlite3 * NGA_NULLABLE db) const noexcept;
        };
        
        struct StmtDeleter final {
            void operator()(struct sqlite3_stmt * NGA_NULLABLE stmt) const noexcept;
        };
        
        typedef std::unique_ptr<struct sqlite3, struct DBDeleter> DBUPtr;
        typedef std::unique_ptr<struct sqlite3_stmt, struct StmtDeleter> StmtUPtr;
        
        DBUPtr _db;
        
        void open(const char * NGA_NONNULL path, const int flags);
        StmtUPtr prepare(const char * NGA_NONNULL sql);
        int step(StmtUPtr & stmt);
        void reuse(StmtUPtr & stmt);
        void execute(const char * NGA_NONNULL sql);
        
    public:
        static uint64_t memoryUsed();
        
        SQLiteDB() noexcept = default;
        ~SQLiteDB() noexcept = default;
    };
    
} // namespace nga

#endif //!__NGA_SQLITE_DB_HPP__
