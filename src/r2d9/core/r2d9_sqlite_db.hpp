/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __R2D9_SQLITE_DB_HPP__
#define __R2D9_SQLITE_DB_HPP__ 1

#include <memory>

#include "../r2d9.hpp"

struct sqlite3;
struct sqlite3_stmt;

namespace r2d9 {
    
    class SQLiteDB {
    protected:
        struct DBDeleter final {
            void operator()(struct sqlite3 * R2D9_NULLABLE db) const noexcept;
        };
        
        struct StmtDeleter final {
            void operator()(struct sqlite3_stmt * R2D9_NULLABLE stmt) const noexcept;
        };
        
        typedef std::unique_ptr<struct sqlite3, struct DBDeleter> DBUPtr;
        typedef std::unique_ptr<struct sqlite3_stmt, struct StmtDeleter> StmtUPtr;
        
        DBUPtr _db;
        
        void open(const char * R2D9_NONNULL path, const int flags);
        StmtUPtr prepare(const char * R2D9_NONNULL sql);
        int step(StmtUPtr & stmt);
        void reuse(StmtUPtr & stmt);
        void execute(const char * R2D9_NONNULL sql);
        
    public:
        static uint64_t memoryUsed();
        
        SQLiteDB() noexcept = default;
        ~SQLiteDB() noexcept = default;
    };
    
} // namespace r2d9

#endif //!__R2D9_SQLITE_DB_HPP__
