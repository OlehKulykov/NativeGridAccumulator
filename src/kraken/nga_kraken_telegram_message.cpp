/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <stdexcept>
#include <format>

#include "nga_kraken_types.hpp"
#include "nga_kraken_decimal_utils.hpp"
#include "../core/nga_time_utils.hpp"
#include "../core/nga_c_string.h"

#include "nga_kraken_telegram_message.hpp"

namespace nga {
namespace kraken {
    
    String TelegramMessage::orderString(const DBOrder & order) {
        char buff0[maxDecimalCStringLen], buff1[maxDecimalCStringLen];
        StringStream stream;
        stream << "[<code>" << order.id;
        if (order.parentId > 0) {
            stream << "/" << order.parentId;
        }
        stream << "</code>] <b>" << OrderTypeToKey(order.type) << "</b> order <b>" << order.pair << "</b>" << '\n';
        
        decimalToCString(order.volume, buff0);
        decimalToCString(order.price, buff1);
        stream << "<code>" << buff0 << "</code> @ <code>" << buff1 << "</code> ";
        
        const Decimal decZero(0);
        if ((order.cost > decZero) && (order.fee >= decZero)) {
            decimalToCString((order.cost + order.fee), buff0);
            stream << "\nTotal <code>" << buff0 << "</code> <b> ";
            if (order.fee == decZero) {
                stream << "≈";
            } else {
                stream << '=';
            }
            stream << " </b>";
            decimalToCString(order.cost, buff0);
            decimalToCString(order.fee, buff1);
            stream << "cost <code>" << buff0 << "</code> <b> + </b>fee <code>" << buff1 << "</code> ";
        }
        
        return stream.str();
    }
    
    String TelegramMessage::profitString(const OrderProfit & profit, const String & pair) {
        char buff0[maxDecimalCStringLen], buff1[maxDecimalCStringLen];
        decimalToCString(profit.volume, buff0);
        decimalToCString(profit.cost, buff1);
        StringStream stream;
        stream << "<b>" << OrderTypeToKey(profit.from) << "→" << OrderTypeToKey(profit.to) << "</b> ";
        stream << "volume <code>" << buff0 << "</code> and cost <code>" << buff1 << "</code> ";
        return stream.str();
    }
    
    TelegramMessage & TelegramMessage::onClosed(const DBOrder & order) {
        StringStream stream;
        if (!empty()) {
            stream << '\n';
        }
        stream << "✅ Closed " << orderString(order);
        addBlock(stream.str());
        return *this;
    }
    
    TelegramMessage & TelegramMessage::addClosedProfit(const OrderProfit & profit, const String & pair) {
        StringStream stream;
        stream << "\n\n" << "🪎 Profit " << profitString(profit, pair);
        addBlock(stream.str());
        return *this;
    }
    
    TelegramMessage & TelegramMessage::onCreated(const DBOrder & order) {
        StringStream stream;
        if (!empty()) {
            stream << '\n';
        }
        stream << "⏳ Created " << orderString(order);
        addBlock(stream.str());
        return *this;
    }
    
    TelegramMessage & TelegramMessage::addABIRate(const Trio<Decimal, Decimal, Decimal> abi, const Decimal rate) {
        char buff0[maxDecimalCStringLen];
        StringStream stream;
        decimalToCString(abi.first, buff0);
        stream << "\n\n" << "📊 Ask <code>" << buff0 << "</code>";
        
        decimalToCString(abi.third, buff0);
        stream << " | index <code>" << buff0 << "</code>";
        
        decimalToCString(abi.second, buff0);
        stream << " | bid <code>" << buff0 << "</code>";
        
        decimalToCString(rate, buff0, maxDecimalCStringLen, 2);
        stream << ". Rate<b> ≈ </b><code>" << buff0 << "</code> %";
        
        addBlock(stream.str());
        return *this;
    }
    
    TelegramMessage & TelegramMessage::addDetails(const DBOrder & order) {
        char buff0[maxDecimalCStringLen], buff1[maxDecimalCStringLen];
        StringStream stream;
        stream << "\n\n" << "📋 Details: <blockquote><pre>";
        stream << "id/parent:    " << order.id << " / " << order.parentId << '\n';
        stream << "tx/cl id:     " << (order.txId.c_str() ?: emptyCString) << " / " << (order.clientId.c_str() ?: emptyCString) << '\n';
        
        decimalToCString(order.volume, buff0);
        decimalToCString(order.price, buff1);
        stream << "volume@price: " << buff0 << " @ " << buff1 << '\n';
        
        const Decimal decZero(0);
        if ((order.cost > 0) && (order.fee >= 0)) {
            decimalToCString(order.cost, buff0);
            decimalToCString(order.fee, buff1);
            stream << "cost+fee:     " << buff0 << " + " << buff1 << '\n';
        }
        
        decimalToCString(order.openTimestamp, buff0);
        auto timePoint = std::chrono::system_clock::from_time_t(static_cast<time_t>(order.openTimestamp));
        timePointToString(timePoint, buff1, maxDecimalCStringLen, TimePointFormatDefaultUTC | TimePointFormatSpaceSep);
        stream << "open ts/date: " << buff0 << " / " << buff1 << '\n';
        stream << "status|type:  " << OrderStatusToKey(order.status) << " | " << OrderTypeToKey(order.type);
        
        stream << "</pre></blockquote>";
        addBlock(stream.str());
        return *this;
    }
    
    TelegramMessage & TelegramMessage::onUpdated(const DBOrder & before, const DBOrder & after) {
        StringStream stream;
        if (!empty()) {
            stream << '\n';
        }
        stream << "⚠️ Updated " << orderString(after) << "\n\n";
        stream << "Previous " << orderString(before);
        addBlock(stream.str());
        return *this;
    }
    
    TelegramMessage & TelegramMessage::addSeparator() {
        addBlock("\n<code>───────────────────</code>");
        return *this;
    }
    
} // namespace kraken
} // namespace nga
