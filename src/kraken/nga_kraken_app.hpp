/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_APP_HPP__
#define __NGA_KRAKEN_APP_HPP__ 1

#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <map>

#include "../core/nga_logger.hpp"
#include "nga_kraken_config.hpp"
#include "nga_kraken_db_order.hpp"
#include "nga_kraken_order_settings_base.hpp"
#include "nga_kraken_icalc.hpp"
#include "nga_kraken_api.hpp"

namespace nga {
namespace kraken {
    
    class NGA_CPP_CLASS_API App final {
    private:
        typedef std::vector<DBOrder> DBOrders;
        typedef std::vector<Order> APIOrders;
        typedef std::pair<DBOrders, DBOrders> MergedDBOrders;
        typedef std::pair<DBOrder, DBOrder> DBOrderParent;
        
        struct PairData final : public OrderSettingsBase {
            DBOrders orders;
            std::unique_ptr<ICalc> calc;
            AskBidIndex abi;
            
            PairData & operator = (const OrderSettingsBase & osb) noexcept {
                OrderSettingsBase::operator=(osb);
                return *this;
            }
            
            PairData(const OrderSettingsBase & osb) noexcept : OrderSettingsBase(osb) { }
        };
        
        std::mutex _mutex;
        std::thread _thread;
        std::condition_variable _condition;
        std::shared_ptr<Logger> _logger;
        std::shared_ptr<ReusableMT<DataVector> > _reusable;
        std::map<String, PairData> _datas;
        String _ordersBDPath;
        std::mutex _syncMutex;
        std::unique_ptr<API> _api;
        std::unique_ptr<Config> _config;
        std::atomic<bool> _working{false};
        std::atomic<bool> _busy{true};
        std::atomic<bool> _unsyncConfig{true};
        std::pair<uint32_t, uint32_t> _checkOrdersTicks;
        std::pair<uint32_t, uint32_t> _updateAskBidTicks;
        std::pair<uint32_t, uint32_t> _updateOrdersInfoTicks;
        int64_t _nextABITime;
        int64_t _nextUOITime;
        int64_t _nextCOTime;
        bool _unsyncOrders;
        
        Order fetchOpenOrder(const String &, const size_t = 10);
        
        Order nextInversedSell(const PairData &, const DBOrder &);
        Order nextInversedBuy(const PairData &, const DBOrder &, const DBOrder &);
        DBOrder createInversed(const PairData &, const DBOrder &, const DBOrder &);
        void createInversed(const std::vector<DBOrderParent> &);
        void processMissedOrders(DBOrders &&);
        DBOrders missedEnabledOrders(APIOrders &&);
        
        void checkOrders();
        void updateOrdersInfos();
        void syncConfig();
        void syncOrders();
        void updateABI(PairData &, const String &);
        void updateABI();
        
        void sync(Config &&);
        void work(int64_t currTime);
        void workerMethod();
        void stopWorking() noexcept;
        
        MergedDBOrders mergeOrders(APIOrders &&, APIOrders &&, DBOrders &&);
        
        static void sync(std::map<String, PairData> &, const std::map<String, OrderSettingsBase> &);
        static int64_t currentTimeMilli();
        
    public:
        void tick() noexcept;
        void init(Config &&, const std::shared_ptr<Logger> &);
        
        void onChanged(Config &&) noexcept;
        void onExit() noexcept;
        
        App() noexcept;
        ~App() noexcept;
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_APP_HPP__
