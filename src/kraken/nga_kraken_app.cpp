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
#include <limits>
#include <inttypes.h>
#include <numeric>
#include <chrono>

#include "nga_kraken_orders_db.hpp"
#include "nga_kraken_calc.hpp"
#include "nga_kraken_decimal_utils.hpp"
#include "../core/nga_randomizer.hpp"

#include "nga_kraken_app.hpp"

namespace nga {
namespace kraken {
    
    //MARK: Template helpers
    
    template<typename T>
    static const T * findOrderPtr(const T * data, const size_t dataSize, const String & clOrTxId) noexcept {
        for (size_t i = 0; i < dataSize; i++, data++) {
            if ( (data->clientId.size() && (clOrTxId == data->clientId)) ||
                 (data->txId.size() && (clOrTxId == data->txId)) ) {
                return data;
            }
        }
        return nullptr;
    }
    
    template<typename T>
    static void removeOrder(std::vector<T> & orders, const String & clOrTxId) {
        for (auto it = orders.begin(); it != orders.end(); ++it) {
            if ( (it->clientId.size() && (clOrTxId == it->clientId)) ||
                 (it->txId.size() && (clOrTxId == it->txId)) ) {
                orders.erase(it);
                return;
            }
        }
    }
    
    template<typename T>
    static const T * findOrder(const std::vector<T> & orders, const String & clOrTxId) noexcept {
        return findOrderPtr(orders.data(), orders.size(), clOrTxId);
    }
    
    template<typename T>
    static T * findOrder(std::vector<T> & orders, const String & clOrTxId) noexcept {
        return const_cast<T *>(findOrderPtr(orders.data(), orders.size(), clOrTxId));
    }
    
    template<typename T>
    static bool compareOrdersByOpenTimestamp(const T & a, const T & b) noexcept {
        return a.openTimestamp < b.openTimestamp;
    }
    
    template<typename T>
    static time_t earliestOrderOpenTimestamp(const std::vector<T> & orders) {
        auto it = std::min_element(orders.begin(), orders.end(), compareOrdersByOpenTimestamp<T>);
        if (it != orders.end()) {
            return std::max<time_t>(0, static_cast<time_t>(it->openTimestamp));
        }
        return 0;
    }
    
    Order App::fetchOpenOrder(const String & clientOrderId, const size_t attempts) {
        std::exception_ptr exception = nullptr;
        size_t i = 1;
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(i * 200)); // Kraken may open order a bit later
            try {
                return _api->fetchOpenOrder(clientOrderId);
            } catch (...) {
                exception = std::current_exception();
            }
        } while (i++ < attempts);
        if (exception) {
            std::rethrow_exception(exception);
        }
        throw std::runtime_error(std::format("App: failed to fetch open order, client id: {}", clientOrderId.c_str()));
    }
    
    Order App::nextInversedSell(const PairData & data, const DBOrder & closedBuy) {
        const Decimal decZero(0);
        const auto volume = closedBuy.volume * data.sellVolumeRate;
        const auto cost = (closedBuy.cost + closedBuy.fee) * data.sellCostRate; // spent = cost + fee
        
        auto nextInfo = data.calc->nextInfo(closedBuy.price, volume, cost, data.step);
        
        if ((data.abi.first > decZero) && (data.abi.first > nextInfo.first)) {
            const auto calculated = decimalToString(nextInfo.first);
            nextInfo.first = data.abi.first;
            _logger->log(loggerTypeDebug, "  Using best ask, calculated: %s, best: %s", calculated.c_str(), decimalToString(nextInfo.first).c_str()); // Price/fee might be changed
        }
        
        Order sellOrder;
        sellOrder.type = OrderType::sell;
        sellOrder.pair = closedBuy.pair;
        sellOrder.price = nextInfo.first;
        sellOrder.volume = nextInfo.second;
        sellOrder.cost = nextInfo.third;
        sellOrder.fee = decZero; // already included in next 'cost' calculation
        return sellOrder;
    }
    
    Order App::nextInversedBuy(const PairData & data, const DBOrder & closedSell, const DBOrder & parentBuy) {
        const Decimal decZero(0);
        const bool isParentValid =
            (parentBuy.id == closedSell.parentId) &&
            (parentBuy.pair == closedSell.pair) &&
            (parentBuy.volume > decZero) &&
            (parentBuy.cost > decZero) &&
            (parentBuy.fee >= decZero) &&
            (parentBuy.type == OrderType::buy) &&
            (parentBuy.status == OrderStatus::closed);
        
        const Decimal cost = (closedSell.cost - closedSell.fee) * data.buyCostRate; // earned = cost - fee
        Decimal volume;
        
        if (isParentValid) {
            volume = parentBuy.volume * data.buyVolumeRate;
        } else {
            volume = closedSell.volume * data.buyVolumeRate;
        }
        
        auto nextInfo = data.calc->nextInfo(closedSell.price, volume, cost, (data.step * Decimal(-1)));
        
        if ((data.abi.second > decZero) && (data.abi.second < nextInfo.first)) {
            const auto calculated = decimalToString(nextInfo.first);
            nextInfo.first = data.abi.second;
            _logger->log(loggerTypeDebug, "  Using best bid, calculated: %s, best: %s", calculated.c_str(), decimalToString(nextInfo.first).c_str()); // Price/fee might be changed
        }
        
        Order buyOrder;
        buyOrder.type = OrderType::buy;
        buyOrder.pair = closedSell.pair;
        buyOrder.price = nextInfo.first;
        buyOrder.volume = nextInfo.second;
        buyOrder.cost = nextInfo.third;
        buyOrder.fee = decZero; // already included in next 'cost' calculation
        return buyOrder;
    }
    
    DBOrder App::createInversed(const PairData & data, const DBOrder & closed, const DBOrder & parent) {
        const Decimal decZero(0);
        const bool isClosedValid =
            (!closed.pair.empty()) &&
            (closed.volume > decZero) &&
            (closed.cost > decZero) &&
            (closed.fee >= decZero) &&
            (closed.price > decZero) &&
            (closed.type != OrderType{0}) &&
            (closed.status != OrderStatus{0});
        if (!isClosedValid) {
            throw std::logic_error(std::format("App: invalid order to create inversed: {}", Order::description(closed).str()));
        }
        
        Order nextInversed;
        switch (closed.type) {
            case OrderType::buy:  nextInversed = nextInversedSell(data, closed); break;
            case OrderType::sell: nextInversed = nextInversedBuy(data, closed, parent); break;
            default: break;
        }
        if (nextInversed.type == OrderType{0}) {
            throw std::runtime_error("App: invalid next inversed order");
        }
        
        _logger->log(loggerTypeDebug, "  Creating order: %s", Order::description(nextInversed).str().c_str()); // Price/fee might be changed
#if defined(DEBUG)
        DBOrder nextOrder = _api->addOrder(nextInversed, true);
#else
        DBOrder nextOrder = _api->addOrder(nextInversed, false);
#endif
        nextOrder.parentId = closed.id;
        nextOrder.createTimestamp = nextOrder.updateTimestamp = ::time(nullptr);
        nextOrder.status = OrderStatus::pending;
        return nextOrder;
    }
    
    void App::createInversed(const std::vector<DBOrderParent> & closedPairs) {
        OrdersDB db;
        db.open(_ordersBDPath.c_str());
        for (const auto & pair : closedPairs) {
            auto it = _datas.find(pair.first.pair);
            if (it == _datas.end()) {
                continue;
            }
            
            PairData & data = it->second;
            const DBOrder & closed = pair.first;
            const DBOrder & parent = pair.second;
            
            _logger->log(loggerTypeInfo, nullptr);
            _logger->log(loggerTypeInfo, "On closed: %s", DBOrder::description(closed).str().c_str());
            if (parent.id > 0) {
                _logger->log(loggerTypeInfo, "  Parent: %s", DBOrder::description(parent).str().c_str());
                _logger->log(loggerTypeInfo, "  Profit: %s", data.calc->profit(closed, parent).description().c_str());
            }
            
            try {
                updateABI(data, closed.pair);
                auto createdMin = createInversed(data, closed, parent);
                createdMin.id = db.insert(createdMin);
                db.insertReplace(closed);
                removeOrder(data.orders, closed.clOrTxId());
                _logger->log(loggerTypeInfo, "  Order created: %s", Order::description(createdMin).str().c_str());
                _logger->log(loggerTypeInfo, "  Index rate: %s %%. Expected profit: %s", decimalToString(createdMin.price * Decimal(100) / closed.price, 4).c_str(), data.calc->profit(createdMin, closed).description().c_str());
                
                DBOrder createdFull = fetchOpenOrder(createdMin.clientId); // delayed with attempts
                createdFull.id = createdMin.id;
                createdFull.parentId = createdMin.parentId;
                createdFull.createTimestamp = createdMin.createTimestamp;
                createdFull.updateTimestamp = createdMin.updateTimestamp;
                
                db.insertReplace(createdFull);
                _logger->log(loggerTypeInfo, "Done. Next: %s", DBOrder::description(createdFull).str().c_str());
                
                data.orders.emplace_back(std::move(createdFull));
            } catch (...) {
                _unsyncOrders = true;
                throw;
            }
        }
    }
    
    void App::processMissedOrders(DBOrders && missed) {
        std::vector<DBOrderParent> closed;
        {
            auto dbOrders = std::move(missed);
            auto apiClosed = _api->fetchClosedOrders();
            OrdersDB db;
            db.open(_ordersBDPath.c_str());
            for (auto & dbOrder : dbOrders) {
                auto * apiOrderPtr = findOrder(apiClosed, dbOrder.clOrTxId());
                if (apiOrderPtr) {
                    dbOrder = std::move(*apiOrderPtr);
                } else {
                    _unsyncOrders = true;
                    continue;
                }
                
                if (dbOrder.status == OrderStatus::closed) {
                    DBOrderParent op;
                    if (dbOrder.parentId > 0) {
                        op.second = db.selectById(dbOrder.parentId);
                    }
                    op.first = std::move(dbOrder);
                    closed.emplace_back(std::move(op));
                    continue;
                }
                
                db.insertReplace(dbOrder);
                
                switch (dbOrder.status) {
                    case OrderStatus::open:
                        _unsyncOrders = true;
                        break;
                        
                    case OrderStatus::canceled:
                    case OrderStatus::expired: {
                        auto it = _datas.find(dbOrder.pair);
                        if (it != _datas.end()) {
                            const auto removedOrderDescr = DBOrder::description(dbOrder).str();
                            removeOrder(it->second.orders, dbOrder.clOrTxId());
                            _logger->log(loggerTypeInfo, "Removed canceled | expired order: %s", removedOrderDescr.c_str());
                        }
                    } break;
                        
                    case OrderStatus::pending:
                    default:
                        break;
                }
            }
        }
        if (closed.size()) {
            createInversed(closed);
        }
    }
    
    App::DBOrders App::missedEnabledOrders(APIOrders && apiOpen) {
        DBOrders missed;
        for (auto it = _datas.begin(); it != _datas.end(); it++) {
            if (it->second.enabled) {
                for (const auto & dbOrder : it->second.orders) {
                    if (!findOrder(apiOpen, dbOrder.clOrTxId())) {
                        missed.emplace_back(DBOrder::copy(dbOrder));
                    }
                }
            }
        }
        return missed;
    }
    
    void App::checkOrders() {
        auto missed = missedEnabledOrders(_api->fetchOpenOrders());
        if (missed.size()) {
            processMissedOrders(std::move(missed));
        }
    }
    
    void App::updateOrdersInfos() {
        auto apiOpen = _api->fetchOpenOrders();
        DBOrders updated;
        for (auto & apiOrder : apiOpen) {
            auto it = _datas.find(apiOrder.pair);
            if (it == _datas.end()) {
                continue;
            }
            auto * dbOrderPtr = findOrder(it->second.orders, apiOrder.clOrTxId());
            if (dbOrderPtr) {
                if ((apiOrder.volume != dbOrderPtr->volume) || (apiOrder.cost != dbOrderPtr->cost) || (apiOrder.fee != dbOrderPtr->fee) || (apiOrder.price != dbOrderPtr->price)) {
                    _logger->log(loggerTypeWarning, nullptr);
                    _logger->log(loggerTypeWarning, "Order updated from: %s", DBOrder::description(*dbOrderPtr).str().c_str());
                    *dbOrderPtr = std::move(apiOrder);
                    dbOrderPtr->updateTimestamp = ::time(nullptr);
                    _logger->log(loggerTypeWarning, "Order updated   to: %s", DBOrder::description(*dbOrderPtr).str().c_str());
                    updated.emplace_back(DBOrder::copy(*dbOrderPtr));
                }
            } else {
                _nextCOTime = -1;
            }
        }
        if (updated.size()) {
            OrdersDB(_ordersBDPath.c_str()).insertReplace(updated);
        }
    }
    
    void App::updateABI(PairData & data, const String & pair) {
        try {
            const auto ab = _api->fetchBestAskBid(pair);
            data.abi.first = ab.first;
            data.abi.second = ab.second;
            data.abi.third = (ab.first + ab.second) / Decimal(2);
        } catch (...) {
            data.abi.first = data.abi.second = data.abi.third = Decimal(-1);
            throw;
        }
    }
    
    void App::updateABI() {
        for (auto it = _datas.begin(); it != _datas.end(); it++) {
            updateABI(it->second, it->first);
        }
    }
    
    void App::syncOrders() {
        time_t earliestOpenTimestamp;
        OrdersDB db;
        {
            auto apiClosed = _api->fetchClosedOrders();
            auto apiOpen = _api->fetchOpenOrders();
            earliestOpenTimestamp = earliestOrderOpenTimestamp(apiOpen);
            const time_t earliest = std::min<time_t>(earliestOpenTimestamp, earliestOrderOpenTimestamp(apiClosed));
            db.open(_ordersBDPath.c_str());
            auto merged = mergeOrders(std::move(apiOpen), std::move(apiClosed), db.selectFromCreateTimestamp(earliest));
            if (merged.first.size()) {
                std::sort(merged.first.begin(), merged.first.end(), compareOrdersByOpenTimestamp<DBOrder>);
                const auto insertTimestamp = ::time(nullptr);
                const Decimal decZero(0);
                for (auto & dbOrder : merged.first) {
                    if (dbOrder.openTimestamp > decZero) {
                        dbOrder.createTimestamp = dbOrder.updateTimestamp = static_cast<int64_t>(dbOrder.openTimestamp);
                    } else {
                        dbOrder.createTimestamp = dbOrder.updateTimestamp = insertTimestamp;
                    }
                    db.insert(dbOrder);
                }
            }
            if (merged.second.size()) {
                db.insertReplace(merged.second);
            }
        }
        
        auto dbOrders = db.selectFromCreateTimestamp(earliestOpenTimestamp);
        for (auto it = _datas.begin(); it != _datas.end(); it++) {
            it->second.orders.clear();
            it->second.orders.shrink_to_fit();
        }
        
        uint64_t openOrdersCount = 0;
        for (auto & dbOrder : dbOrders) {
            if (dbOrder.status == OrderStatus::open) {
                auto it = _datas.find(dbOrder.pair);
                if (it != _datas.end()) {
                    it->second.orders.emplace_back(std::move(dbOrder));
                    openOrdersCount++;
                }
            }
        }
        
        _logger->log(loggerTypeInfo, nullptr);
        _logger->log(loggerTypeInfo, "Sync. Tracking orders count: %" PRIu64, openOrdersCount);
        for (auto it = _datas.begin(); it != _datas.end(); it++) {
            const size_t pairOrdersCount = it->second.orders.size();
            if (pairOrdersCount && it->second.enabled) {
                std::stringstream pairStream;
                pairStream << "  " << it->first << " (" << it->second.orders.size() << ") [";
                for (size_t i = 0; i < pairOrdersCount; i++) {
                    if (i) {
                        pairStream << ", ";
                    }
                    pairStream << it->second.orders[i].id;
                }
                pairStream << ']';
                _logger->log(loggerTypeInfo, pairStream.str().c_str());
            }
        }
        _logger->log(loggerTypeInfo, "Done.");
    }
    
    void App::syncConfig() {
        const std::lock_guard<std::mutex> lock(_syncMutex);
        auto config = std::move(_config);
        if (config) {
            sync(std::move(*config.get()));
        }
    }
    
    void App::work(int64_t currTime) {
        if (_unsyncConfig) {
            _busy = true;
            syncConfig();
            _unsyncConfig = false;
            currTime = currentTimeMilli();
        }
        
        if (_unsyncOrders) {
            _busy = true;
            syncOrders();
            _unsyncOrders = false;
            currTime = currentTimeMilli();
        }
        
        if (currTime >= _nextABITime) {
            _busy = true;
            updateABI();
            _nextABITime = (currTime = currentTimeMilli()) + randomInRange<int64_t>(_updateAskBidTicks.first, _updateAskBidTicks.second);
        }
        
        if (currTime >= _nextUOITime) {
            _busy = true;
            updateOrdersInfos();
            _nextUOITime = (currTime = currentTimeMilli()) + randomInRange<int64_t>(_updateOrdersInfoTicks.first, _updateOrdersInfoTicks.second);
        }
        
        if (currTime >= _nextCOTime) {
            _busy = true;
            checkOrders();
            _nextCOTime = (currTime = currentTimeMilli()) + randomInRange<int64_t>(_checkOrdersTicks.first, _checkOrdersTicks.second);
        }
        
        _busy = false;
    }
    
    void App::workerMethod() {
        std::unique_lock<std::mutex> lock(_mutex);
        do {
            _condition.wait(lock);
            
            if (_working) {
                lock.unlock();
                
                try {
                    work(currentTimeMilli());
                } catch (...) {
                    _logger->log(std::current_exception());
                    _unsyncConfig = true;
                    _unsyncOrders = true;
                    _nextABITime = _nextUOITime = _nextCOTime = -1;
                    _busy = false;
                }
                
                lock.lock();
            }
        } while (_working);
    }
    
    void App::stopWorking() noexcept {
        if (_working) {
            _working = false;
            _condition.notify_all();
            try {
                _thread.join();
            } catch (...) {
                // do nothing
            }
        }
    }
    
    void App::sync(Config && config) {
        _api.reset();
        _ordersBDPath.clear();
        
        if (config.apiKey.size() && config.privateKey.size()) {
            if (!_reusable) {
                _reusable = std::make_shared<ReusableMT<DataVector> >();
            }
            _api = std::make_unique<API>(std::move(config.apiKey), std::move(config.privateKey), _reusable);
        }
        if (!_api) {
            throw std::logic_error("App: failed to initialize required Kraken API");
        }
        
        {
            OrdersDB db;
            db.open(config.ordersBD.c_str(), true);
            _ordersBDPath = config.ordersBD.c_str();
        }
        
        _checkOrdersTicks = config.checkOrdersTicks;
        _updateAskBidTicks = config.updateAskBidTicks;
        _updateOrdersInfoTicks = config.updateOrdersInfoTicks;
        
        sync(_datas, config.orderSettings);
    }
    
    void App::tick() noexcept {
        if (_busy) {
            return;
        }
        if (_working) {
            _condition.notify_one();
        }
    }
    
    void App::init(Config && config, const std::shared_ptr<Logger> & logger) {
        if (_working) {
            throw std::logic_error("App: already initialized");
        }
        
        _busy = true;
        _logger = logger;
        _nextABITime = _nextUOITime = _nextCOTime = -1;
        _unsyncOrders = true;
        
        {   // force/inline config synchronization
            const std::lock_guard<std::mutex> lock(_syncMutex);
            _config.reset();
            sync(std::move(config));
            _unsyncConfig = false;
        }
        
        _working = true;
        _thread = std::thread(&App::workerMethod, this);
        _busy = false;
    }
    
    void App::onChanged(Config && config) noexcept {
        try {
            const std::lock_guard<std::mutex> lock(_syncMutex);
            _config = std::make_unique<Config>(std::move(config));
            _unsyncConfig = true;
        } catch (...) {
            _logger->log(std::current_exception());
        }
    }
    
    void App::onExit() noexcept {
        stopWorking();
    }
    
    App::App() noexcept {
        CURLRequest::globalInit();
    }
    
    App::~App() noexcept {
        stopWorking();
        CURLRequest::globalDeinit();
    }
    
    //MARK: Helpers
    
    App::MergedDBOrders App::mergeOrders(APIOrders && apiOpen, APIOrders && apiClosed, DBOrders && dbOrders) {
        App::MergedDBOrders merged;
        for (size_t i = 0; i < 2; i++) {
            auto apiOrders = i ? std::move(apiOpen) : std::move(apiClosed);
            for (auto & apiOrder : apiOrders) {
                auto * dbOrderPtr = findOrder(dbOrders, apiOrder.clOrTxId());
                if (dbOrderPtr) {
                    *dbOrderPtr = std::move(apiOrder);
                    if (dbOrderPtr->createTimestamp < 0) {
                        dbOrderPtr->createTimestamp = ::time(nullptr);
                    }
                    if (dbOrderPtr->updateTimestamp < dbOrderPtr->createTimestamp) {
                        dbOrderPtr->updateTimestamp = dbOrderPtr->createTimestamp;
                    }
                    merged.second.emplace_back(std::move(*dbOrderPtr));
                } else {
                    merged.first.emplace_back(DBOrder(std::move(apiOrder)));
                }
            }
        }
        return merged;
    }
    
    void App::sync(std::map<String, PairData> & datas, const std::map<String, OrderSettingsBase> & settings) {
        for (auto it = datas.begin(); it != datas.end(); ) {
            if (settings.contains(it->first)) {
                it++;
            } else {
                it = datas.erase(it);
            }
        }
        for (auto pair : settings) {
            auto it = datas.find(pair.first);
            if (it == datas.end()) {
                PairData data(pair.second);
                data.calc = std::make_unique<Calc>(data.fee, data.pairDecimals, data.lotDecimals);
                data.abi.first = data.abi.second = data.abi.third = -1;
                const auto res = datas.emplace(pair.first, std::move(data));
                if (!res.second) {
                    throw std::runtime_error("App: failed to insert new pair oder data");
                }
            } else {
                it->second.calc = std::make_unique<Calc>(pair.second.fee, pair.second.pairDecimals, pair.second.lotDecimals);
                it->second = pair.second;
            }
        }
    }
    
    int64_t App::currentTimeMilli() {
        struct timespec tms;
        if (::clock_gettime(CLOCK_MONOTONIC, &tms) != 0) {
            throw std::runtime_error(std::format("App: error retrieve current time, errno: {} ({})", errno, ::strerror(errno)));
        }
        return (static_cast<int64_t>(tms.tv_sec) * 1000) + (tms.tv_nsec / 1000000);
    }
    
} // namespace kraken
} // namespace nga
