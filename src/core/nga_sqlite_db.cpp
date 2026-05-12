/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <stdexcept>
#include <cstdio>

#include "nga_fixed_string_stream.hpp"
#include "nga_c_string.h"

#include <sqlite3.h>

#include "nga_sqlite_db.hpp"

namespace nga {
    
    void SQLiteDB::DBDeleter::operator()(struct sqlite3 * db) const noexcept {
        if (db) {
            ::sqlite3_close_v2(db);
        }
    }
    
    void SQLiteDB::StmtDeleter::operator()(struct sqlite3_stmt * stmt) const noexcept {
        if (stmt) {
            const int res = ::sqlite3_finalize(stmt);
            DEBUG_ASSERT(res == SQLITE_OK)
            (void)res;
        }
    }
    
    void SQLiteDB::open(const char * NGA_NONNULL path, const int flags) {
        DBUPtr db;
        
        sqlite3 * dbPtr = nullptr;
        const int res = ::sqlite3_open_v2(path, &dbPtr, flags, nullptr);
        if (!dbPtr) {
            throw std::runtime_error("SQLite open db");
        }
        db.reset(dbPtr);
        
        if (res != SQLITE_OK) {
            char reason[256];
            ::snprintf(reason, 256, "SQLite open db, result: %i, error: %s", res, (::sqlite3_errmsg(db.get()) ?: emptyCString));
            throw std::runtime_error(reason);
        }
        
        _db = static_cast<DBUPtr &&>(db);
    }
    
    SQLiteDB::StmtUPtr SQLiteDB::prepare(const char * NGA_NONNULL sql) {
        struct sqlite3_stmt * stmtPtr = nullptr;
        const int res = ::sqlite3_prepare_v3(_db.get(), sql, -1, 0, &stmtPtr, nullptr);
        StmtUPtr stmt(stmtPtr);
        if (!stmtPtr || (res != SQLITE_OK)) {
            char reason[256];
            ::snprintf(reason, 256, "SQLite prepare statement, result: %i, error: %s", res, (::sqlite3_errmsg(_db.get()) ?: emptyCString));
            throw std::runtime_error(reason);
        }
        return stmt;
    }
    
    int SQLiteDB::step(StmtUPtr & stmt) {
        const int res = ::sqlite3_step(stmt.get());
        switch (res) {
            case SQLITE_DONE:
            case SQLITE_ROW:
                return res;
        }
        
        if (res != SQLITE_OK) {
            // SQLITE_CONSTRAINT: record already exists
            char reason[256];
            ::snprintf(reason, 256, "SQLite step, result: %i, error: %s", res, (::sqlite3_errmsg(_db.get()) ?: emptyCString));
            throw std::runtime_error(reason);
        }
        
        return res;
    }
    
    void SQLiteDB::reuse(StmtUPtr & stmt) {
        int res = ::sqlite3_reset(stmt.get());
        if (res == SQLITE_OK) {
            res = ::sqlite3_clear_bindings(stmt.get());
        }
        if (res != SQLITE_OK) {
            char reason[256];
            ::snprintf(reason, 256, "SQLite reuse statement, result: %i, error: %s", res, (::sqlite3_errmsg(_db.get()) ?: emptyCString));
            throw std::runtime_error(reason);
        }
    }
    
    void SQLiteDB::execute(const char * NGA_NONNULL sql) {
        char * errMsg = nullptr;
        const int res = ::sqlite3_exec(_db.get(), sql, nullptr, nullptr, &errMsg);
        if (res != SQLITE_OK) {
            char reason[256];
            ::snprintf(reason, 256, "SQLite execute, result: %i, error: %s", res, (errMsg ?: emptyCString));
            std::runtime_error exception(reason);
            if (errMsg) {
                ::sqlite3_free(errMsg);
            }
            throw exception;
        }
    }
    
    uint64_t SQLiteDB::memoryUsed() {
        const auto memUsed = ::sqlite3_memory_used();
        return (memUsed > 0) ? static_cast<uint64_t>(memUsed) : 0;
    }
    
} // namespace nga
