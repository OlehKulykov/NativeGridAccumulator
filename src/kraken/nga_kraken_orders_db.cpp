/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <memory>
#include <stdexcept>

#include <sqlite3.h>

#include "../core/nga_fixed_string_stream.hpp"
#include "../core/nga_scope_guard.hpp"
#include "../core/nga_c_string.h"

#include "nga_kraken_decimal_utils.hpp"

#include "nga_kraken_orders_db.hpp"

namespace nga {
namespace kraken {
    
    OrdersDB & OrdersDB::open(const char * NGA_NONNULL path, const bool init) {
        if (_db) {
            return *this;
        }
        
        SQLiteDB::open(path, static_cast<int>(SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX));
        
        if (init) {
            FixedStringStream<255> stream;
            stream << "CREATE TABLE IF NOT EXISTS " << "orders" << "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
            << "pair" << " TEXT" << ','
            << "volume" << " TEXT" << ','
            << "price" << " TEXT" << ','
            << "cost" << " TEXT" << ','
            << "fee" << " TEXT" << ','
            << "type" << " INTEGER" << ','
            << "status" << " INTEGER" << ','
            << "prntid" << " INTEGER" << ','
            << "crtts" << " INTEGER" << ','
            << "updts" << " INTEGER" << ','
            << "opents" << " TEXT" << ','
            << "clid" << " TEXT" << ','
            << "txid" << " TEXT" << ");";
            execute(stream);
            
            stream.clear();
            stream << "CREATE INDEX IF NOT EXISTS " << "orders_id_index" << " ON " << "orders" << '('
            << "id" << " ASC" << ");";
            execute(stream);
            
            stream.clear();
            stream << "CREATE INDEX IF NOT EXISTS " << "orders_crtts_index" << " ON " << "orders" << '('
            << "crtts" << " ASC" << ");";
            execute(stream);
        }
        
        return *this;
    }
    
    std::vector<DBOrder> OrdersDB::selectFromCreateTimestamp(const int64_t fromTimestamp) {
        FixedStringStream<127> stream;
        stream << "SELECT " << '*' << " FROM " << "orders" << " WHERE " << "crtts" << " >= ?;";
        
        auto stmt = prepare(stream);
        
        int res = ::sqlite3_bind_int64(stmt.get(), 1, fromTimestamp); // 1
        if (res != SQLITE_OK) {
            throw std::runtime_error("Bind");
        }
        
        std::vector<DBOrder> orders;
        while ( (res = step(stmt)) == SQLITE_ROW ) {
            auto * stmtPtr = stmt.get();
            const char * tmpText;
            DBOrder order;
            order.id = ::sqlite3_column_int64(stmtPtr, 0); // 0=id
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 1))) ) { order.pair = tmpText; }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 2))) ) { order.volume = Decimal(tmpText); }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 3))) ) { order.price = Decimal(tmpText); }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 4))) ) { order.cost = Decimal(tmpText); }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 5))) ) { order.fee = Decimal(tmpText); }
            order.type = static_cast<OrderType>(::sqlite3_column_int(stmtPtr, 6));
            order.status = static_cast<OrderStatus>(::sqlite3_column_int(stmtPtr, 7));
            order.parentId = ::sqlite3_column_int64(stmtPtr, 8);
            order.createTimestamp = ::sqlite3_column_int64(stmtPtr, 9);
            order.updateTimestamp = ::sqlite3_column_int64(stmtPtr, 10);
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 11))) ) { order.openTimestamp = Decimal(tmpText); }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 12))) ) { order.clientId = tmpText; }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 13))) ) { order.txId = tmpText; }
            orders.emplace_back(static_cast<DBOrder &&>(order));
        }
        
        switch (res) {
            case SQLITE_DONE:
            case SQLITE_OK:
                return orders;
                
            default:
                throw std::runtime_error("Select");
        }
        return orders;
    }
    
    DBOrder OrdersDB::selectById(const int64_t orderId) {
        FixedStringStream<127> stream;
        stream << "SELECT " << '*' << " FROM " << "orders" << " WHERE " << "id" << " = ?;";
        
        auto stmt = prepare(stream);
        
        int res = ::sqlite3_bind_int64(stmt.get(), 1, orderId); // 1
        if (res != SQLITE_OK) {
            throw std::runtime_error("Bind");
        }
        
        DBOrder order;
        while ( (res = step(stmt)) == SQLITE_ROW ) {
            auto * stmtPtr = stmt.get();
            const char * tmpText;
            order.id = ::sqlite3_column_int64(stmtPtr, 0); // 0=id
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 1))) ) { order.pair = tmpText; }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 2))) ) { order.volume = Decimal(tmpText); }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 3))) ) { order.price = Decimal(tmpText); }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 4))) ) { order.cost = Decimal(tmpText); }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 5))) ) { order.fee = Decimal(tmpText); }
            order.type = static_cast<OrderType>(::sqlite3_column_int(stmtPtr, 6));
            order.status = static_cast<OrderStatus>(::sqlite3_column_int(stmtPtr, 7));
            order.parentId = ::sqlite3_column_int64(stmtPtr, 8);
            order.createTimestamp = ::sqlite3_column_int64(stmtPtr, 9);
            order.updateTimestamp = ::sqlite3_column_int64(stmtPtr, 10);
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 11))) ) { order.openTimestamp = Decimal(tmpText); }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 12))) ) { order.clientId = tmpText; }
            if ( (tmpText = reinterpret_cast<const char *>(::sqlite3_column_text(stmtPtr, 13))) ) { order.txId = tmpText; }
            return order;
        }
        
        switch (res) {
            case SQLITE_DONE:
            case SQLITE_OK:
                return order;
                
            default:
                throw std::runtime_error("Select");
        }
        return order;
    }
    
    int64_t OrdersDB::insert() {
        FixedStringStream<127> stream;
        stream << "INSERT INTO " << "orders" << '(' << "crtts" << ") VALUES(" << '?' << ");";
        auto stmt = prepare(stream);
        if (::sqlite3_bind_int64(stmt.get(), 1, -1) != SQLITE_OK) {
            throw std::runtime_error("Bind");
        }
        step(stmt);
        return ::sqlite3_last_insert_rowid(_db.get());
    }
    
    int64_t OrdersDB::insert(const DBOrder & order) {
        FixedStringStream<255> stream;
        stream << "INSERT INTO " << "orders" << '('
        << "pair" << ','
        << "volume" << ','
        << "price" << ','
        << "cost" << ','
        << "fee" << ','
        << "type" << ','
        << "status" << ','
        << "prntid" << ','
        << "crtts" << ','
        << "updts" << ','
        << "opents" << ','
        << "clid" << ','
        << "txid" << ") VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?);";
        
        char volumeStr[maxDecimalCStringLen],
        priceStr[maxDecimalCStringLen],
        costStr[maxDecimalCStringLen],
        feeStr[maxDecimalCStringLen],
        opntsStr[maxDecimalCStringLen];
        
        decimalToCString(order.volume, volumeStr);
        decimalToCString(order.price, priceStr);
        decimalToCString(order.cost, costStr);
        decimalToCString(order.fee, feeStr);
        decimalToCString(order.openTimestamp, opntsStr);
        
        auto stmt = prepare(stream);
        auto * stmtPtr = stmt.get();
        int res = ::sqlite3_bind_text(stmtPtr, 1, (order.pair.c_str() ?: emptyCString), -1, SQLITE_STATIC);
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 2, volumeStr, -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 3, priceStr, -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 4, costStr, -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 5, feeStr, -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_int(stmtPtr, 6, static_cast<int>(order.type)); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_int(stmtPtr, 7, static_cast<int>(order.status)); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_int64(stmtPtr, 8, order.parentId); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_int64(stmtPtr, 9, order.createTimestamp); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_int64(stmtPtr, 10, order.updateTimestamp); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 11, opntsStr, -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 12, (order.clientId.c_str() ?: emptyCString), -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 13, (order.txId.c_str() ?: emptyCString), -1, SQLITE_STATIC); }
        if (res != SQLITE_OK) {
            throw std::runtime_error("Bind");
        }
        step(stmt);
        return ::sqlite3_last_insert_rowid(_db.get());
    }
    
    void OrdersDB::insertReplace(const DBOrder & order) {
        FixedStringStream<255> stream;
        stream << "INSERT OR REPLACE INTO " << "orders" << '('
        << "id" << ','
        << "pair" << ','
        << "volume" << ','
        << "price" << ','
        << "cost" << ','
        << "fee" << ','
        << "type" << ','
        << "status" << ','
        << "prntid" << ','
        << "crtts" << ','
        << "updts" << ','
        << "opents" << ','
        << "clid" << ','
        << "txid" << ") VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
        
        char volumeStr[maxDecimalCStringLen],
        priceStr[maxDecimalCStringLen],
        costStr[maxDecimalCStringLen],
        feeStr[maxDecimalCStringLen],
        opntsStr[maxDecimalCStringLen];
        
        decimalToCString(order.volume, volumeStr);
        decimalToCString(order.price, priceStr);
        decimalToCString(order.cost, costStr);
        decimalToCString(order.fee, feeStr);
        decimalToCString(order.openTimestamp, opntsStr);
        
        auto stmt = prepare(stream);
        auto * stmtPtr = stmt.get();
        int res = ::sqlite3_bind_int64(stmtPtr, 1, order.id);
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 2, (order.pair.c_str() ?: emptyCString), -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 3, volumeStr, -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 4, priceStr, -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 5, costStr, -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 6, feeStr, -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_int(stmtPtr, 7, static_cast<int>(order.type)); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_int(stmtPtr, 8, static_cast<int>(order.status)); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_int64(stmtPtr, 9, order.parentId); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_int64(stmtPtr, 10, order.createTimestamp); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_int64(stmtPtr, 11, order.updateTimestamp); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 12, opntsStr, -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 13, (order.clientId.c_str() ?: emptyCString), -1, SQLITE_STATIC); }
        if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 14, (order.txId.c_str() ?: emptyCString), -1, SQLITE_STATIC); }
        if (res != SQLITE_OK) {
            throw std::runtime_error("Bind");
        }
        step(stmt);
    }
    
    void OrdersDB::insertReplace(const std::vector<DBOrder> & orders) {
        FixedStringStream<255> stream;
        stream << "INSERT OR REPLACE INTO " << "orders" << '('
        << "id" << ','
        << "pair" << ','
        << "volume" << ','
        << "price" << ','
        << "cost" << ','
        << "fee" << ','
        << "type" << ','
        << "status" << ','
        << "prntid" << ','
        << "crtts" << ','
        << "updts" << ','
        << "opents" << ','
        << "clid" << ','
        << "txid" << ") VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
        
        bool didBegin = false;
        try {
            execute("BEGIN TRANSACTION;");
            didBegin = true;
            auto stmt = prepare(stream);
            for (const auto & order : orders) {
                char volumeStr[maxDecimalCStringLen],
                priceStr[maxDecimalCStringLen],
                costStr[maxDecimalCStringLen],
                feeStr[maxDecimalCStringLen],
                opntsStr[maxDecimalCStringLen];
                
                decimalToCString(order.volume, volumeStr);
                decimalToCString(order.price, priceStr);
                decimalToCString(order.cost, costStr);
                decimalToCString(order.fee, feeStr);
                decimalToCString(order.openTimestamp, opntsStr);
                
                auto * stmtPtr = stmt.get();
                int res = ::sqlite3_bind_int64(stmtPtr, 1, order.id);
                if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 2, (order.pair.c_str() ?: emptyCString), -1, SQLITE_STATIC); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 3, volumeStr, -1, SQLITE_STATIC); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 4, priceStr, -1, SQLITE_STATIC); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 5, costStr, -1, SQLITE_STATIC); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 6, feeStr, -1, SQLITE_STATIC); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_int(stmtPtr, 7, static_cast<int>(order.type)); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_int(stmtPtr, 8, static_cast<int>(order.status)); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_int64(stmtPtr, 9, order.parentId); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_int64(stmtPtr, 10, order.createTimestamp); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_int64(stmtPtr, 11, order.updateTimestamp); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 12, opntsStr, -1, SQLITE_STATIC); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 13, (order.clientId.c_str() ?: emptyCString), -1, SQLITE_STATIC); }
                if (res == SQLITE_OK) { res = ::sqlite3_bind_text(stmtPtr, 14, (order.txId.c_str() ?: emptyCString), -1, SQLITE_STATIC); }
                if (res != SQLITE_OK) {
                    throw std::runtime_error("Bind");
                }
                step(stmt);
                reuse(stmt);
            }
            execute("COMMIT;");
        } catch (...) {
            if (didBegin) {
                execute("ROLLBACK;");
            }
            throw;
        }
    }
    
    OrdersDB::OrdersDB(const char * NGA_NONNULL path, const bool init) : SQLiteDB() {
        open(path, init);
    }
    
} // namespace kraken
} // namespace nga
