/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_TELEGRAM_MESSAGE_HPP__
#define __NGA_KRAKEN_TELEGRAM_MESSAGE_HPP__ 1

#include "nga_kraken_db_order.hpp"
#include "nga_kraken_order_profit.hpp"

#include "../telegram/nga_telegram_message_base.hpp"

namespace nga {
namespace kraken {
    
    class NGA_CPP_CLASS_API TelegramMessage final : public telegram::MessageBase {
    private:
        static String orderString(const DBOrder & order);
        static String profitString(const OrderProfit & profit, const OHLCPair pair);
        
    public:
        void onClosed(const DBOrder & order);
        void addClosedProfit(const OrderProfit & profit, const OHLCPair pair);
        
        void onCreated(const DBOrder & order);
        void addExpectedProfit(const OrderProfit & profit, const OHLCPair pair);
        void addABIRate(const Trio<Decimal, Decimal, Decimal> abi, const Decimal rate);
        void addDetails(const DBOrder & order);
        
        void onUpdated(const DBOrder & before, const DBOrder & after);
        void addUpdatedExpectedProfits(const OrderProfit & before, const OrderProfit & after, const OHLCPair pair);
        
        void addSeparator();
        
        TelegramMessage() noexcept = default;
        virtual ~TelegramMessage() noexcept = default;
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_TELEGRAM_MESSAGE_HPP__
