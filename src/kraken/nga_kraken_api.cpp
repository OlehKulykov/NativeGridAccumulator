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

#include "../core/nga_scope_guard.hpp"
#include "../core/nga_randomizer.hpp"
#include "../core/nga_rapidjson.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/error.h>
#include <rapidjson/error/en.h>

#include "nga_kraken_decimal_utils.hpp"

#include "nga_kraken_api.hpp"

namespace nga {
namespace kraken {
    
    ///@link https://docs.kraken.com/api/docs/rest-api/get-tradable-asset-pairs
    std::vector<AssetPairBase> API::tradableAssetPairs() {
        auto reusable = _reusable ? _reusable->get() : nullptr;
        ScopeGuard guard([=] {
            if (reusable) {
                crypto::zeroFill(*reusable);
            }
        });
        
        auto json = requestPublic("AssetPairs",
                                  std::move(reusable),
                                  nullptr, nullptr);
        
        return parseTradableAssetPairs(json ? json->data() : nullptr);
    }
    
    ///@link https://docs.kraken.com/api/docs/rest-api/get-open-orders
    Order API::fetchOpenOrder(const String & clientOrderId) {
        auto reusable = _reusable ? _reusable->get() : nullptr;
        ScopeGuard guard([=] {
            if (reusable) {
                crypto::zeroFill(*reusable);
            }
        });
        
        const char * clOrdId = clientOrderId.c_str() ?: emptyCString;
        auto json = requestPrivate("OpenOrders",
                                   std::move(reusable),
                                   "cl_ord_id", clOrdId, nullptr);
        
        auto orders = parseOrders(json ? json->data() : nullptr, "open");
        for (auto & order : orders) {
            if (clientOrderId == order.clientId) {
                return std::move(order);
            }
        }
        
        throw std::runtime_error(std::format("API: open order not found, clientId: {}", clOrdId));
    }
    
    ///@link https://docs.kraken.com/api/docs/rest-api/get-open-orders
    std::vector<Order> API::fetchOpenOrders() {
        auto reusable = _reusable ? _reusable->get() : nullptr;
        ScopeGuard guard([=] {
            if (reusable) {
                crypto::zeroFill(*reusable);
            }
        });
        
        auto json = requestPrivate("OpenOrders",
                                   std::move(reusable),
                                   nullptr, nullptr);
        
        return parseOrders(json ? json->data() : nullptr, "open");
    }
    
    ///@link https://docs.kraken.com/api/docs/rest-api/get-closed-orders
    Order API::fetchClosedOrder(const String & clientOrderId) {
        auto reusable = _reusable ? _reusable->get() : nullptr;
        ScopeGuard guard([=] {
            if (reusable) {
                crypto::zeroFill(*reusable);
            }
        });
        
        const char * clOrdId = clientOrderId.c_str() ?: emptyCString;
        auto json = requestPrivate("ClosedOrders",
                                   std::move(reusable),
                                   "cl_ord_id", clOrdId, nullptr);
        
        auto orders = parseOrders(json ? json->data() : nullptr, "closed");
        for (auto & order : orders) {
            if (clientOrderId == order.clientId) {
                return std::move(order);
            }
        }
        
        throw std::runtime_error(std::format("API: closed order not found, clientId: {}", clOrdId));
    }
    
    ///@link https://docs.kraken.com/api/docs/rest-api/get-closed-orders
    std::vector<Order> API::fetchClosedOrders() {
        auto reusable = _reusable ? _reusable->get() : nullptr;
        ScopeGuard guard([=] {
            if (reusable) {
                crypto::zeroFill(*reusable);
            }
        });
        
        auto json = requestPrivate("ClosedOrders",
                                   std::move(reusable),
                                   nullptr, nullptr);
        
        return parseOrders(json ? json->data() : nullptr, "closed");
    }
    
    ///@link https://docs.kraken.com/api/docs/rest-api/get-order-book
    AskBid API::fetchBestAskBid(const String & pair) {
        auto reusable = _reusable ? _reusable->get() : nullptr;
        ScopeGuard guard([=] {
            if (reusable) {
                crypto::zeroFill(*reusable);
            }
        });
        
        const char * pairCStr = pair.c_str() ?: emptyCString;
        auto json = requestPublic("Depth",
                                  std::move(reusable),
                                  "pair", pairCStr,
                                  "count", "1", nullptr);
        
        return parseBestAskBid(json ? json->data() : nullptr, pairCStr);
    }
    
    ///@link https://docs.kraken.com/api/docs/rest-api/add-order
    Order API::addOrder(const Order & sourceOrder, const bool validateOnly) {
        auto reusable = _reusable ? _reusable->get() : nullptr;
        ScopeGuard guard([=] {
            if (reusable) {
                crypto::zeroFill(*reusable);
            }
        });
        
        const auto clientOrderId = generateClientOrderId();
        const char * type = (sourceOrder.type == OrderType::buy) ? orderTypeBuy : orderTypeSell;
        const char * validate = validateOnly ? "true" : "false";
        const char * pair = sourceOrder.pair.c_str() ?: emptyCString;
        
        char volume[maxDecimalCStringLen], price[maxDecimalCStringLen];
        
        decimalToCString(sourceOrder.volume, volume);
        decimalToCString(sourceOrder.price, price);
        
        auto json = requestPrivate("AddOrder",
                                   std::move(reusable),
                                   "ordertype", "limit",
                                   "oflags", "post",
                                   "type", type,
                                   "cl_ord_id", clientOrderId.data(),
                                   "pair", pair,
                                   "volume", static_cast<const char *>(volume),
                                   "price", static_cast<const char *>(price),
                                   "validate", validate, nullptr);
        
#if defined(NGA_OUTPUT) || defined(DEBUG)
        if (validate) {
            const char * validationResult = json ? reinterpret_cast<char *>(json->data()) : nullptr;
            std::flush(std::cout) << "Add order validation result: " << (validationResult ?: emptyCString) << std::endl;
        }
#endif
        
        const auto txId = parseAddOrderTransactionId(json ? json->data() : nullptr);
        
        Order order = Order::copy(sourceOrder);
        order.txId = std::move(txId);
        order.clientId = clientOrderId.data();
        return order;
    }
    
    ///@link https://docs.kraken.com/api/docs/rest-api/amend-order
    ///@link https://docs.kraken.com/api/docs/guides/spot-amends
    void API::amendOrder(const Order & sourceOrder) {
        auto reusable = _reusable ? _reusable->get() : nullptr;
        ScopeGuard guard([=] {
            if (reusable) {
                crypto::zeroFill(*reusable);
            }
        });
        
        if (sourceOrder.status != OrderStatus::open) {
            throw std::invalid_argument("API: Order not opened");
        }
        
        const char * txidOrClOrdIdParam = nullptr, * txidOrClOrdIdValue = nullptr;
        if (sourceOrder.clientId.empty()) {
            txidOrClOrdIdParam = "txid";
            txidOrClOrdIdValue = sourceOrder.txId.empty() ? nullptr : sourceOrder.txId.c_str();
        } else {
            txidOrClOrdIdParam = "cl_ord_id";
            txidOrClOrdIdValue = sourceOrder.clientId.c_str();
        }
        if (!txidOrClOrdIdValue) {
            throw std::invalid_argument("API: no order transaction or client identifier");
        }
        
        const char * pair = sourceOrder.pair.c_str() ?: emptyCString;
        char orderQuantity[maxDecimalCStringLen], limitPrice[maxDecimalCStringLen];
        
        decimalToCString(sourceOrder.volume, orderQuantity);
        decimalToCString(sourceOrder.price, limitPrice);
        
        auto json = requestPrivate("AmendOrder",
                                   std::move(reusable),
                                   txidOrClOrdIdParam, txidOrClOrdIdValue,
                                   "order_qty", static_cast<const char *>(orderQuantity),
                                   "limit_price", static_cast<const char *>(limitPrice),
                                   "pair", pair,
                                   "post_only", "true", nullptr);
        
        const auto amendId = parseAmendOrderId(json ? json->data() : nullptr);
        if (amendId.empty()) {
            throw std::runtime_error("API: unsuccessful amend");
        }
    }
    
    API::API(String && apiKey, String && privateKey,
             const std::shared_ptr<ReusableMT<DataVector> > & reusable) : APIBase(std::move(apiKey), std::move(privateKey)),
        _reusable(reusable) {
        
    }
    
    API::API(const std::shared_ptr<ReusableMT<DataVector> > & reusable) : APIBase(),
        _reusable(reusable) {
        
    }
    
    using namespace RAPIDJSON_NAMESPACE;
    
    static RAPIDJSON_NAMESPACE::Document APICreateDocument(uint8_t * NGA_NONNULL jsonData) {
        Document doc;
        doc.ParseInsitu<kParseStopWhenDoneFlag | kParseCommentsFlag | kParseTrailingCommasFlag>(reinterpret_cast<char *>(jsonData));
        if (doc.HasParseError()) {
            throw std::runtime_error(std::format("API: document parse error: \'{}\', offset: {}", GetParseError_En(doc.GetParseError()) ?: "Unknown", doc.GetErrorOffset()));
        }
        if (!doc.IsObject()) {
            throw std::runtime_error("API: document not an object");
        }
        
        auto & errorArray = findArray(doc, "error", emptyArrayValue);
        if (errorArray.Size()) {
            StringStream fallbackStream;
            fallbackStream << "API: errors: [";
            for (size_t i = 0, j = 0, n = errorArray.Size(); i < n; i++) {
                const auto & error = errorArray[i];
                if (error.IsString()) {
                    const char * errorCStr = error.GetString() ?: emptyCString;
                    APIError apiError(errorCStr);
                    if (apiError.severity() != APIErrorSeverity{0}) {
                        throw apiError;
                    }
                    if (j++) {
                        fallbackStream << ',';
                    }
                    fallbackStream << errorCStr;
                }
            }
            fallbackStream << ']';
            throw APIError(fallbackStream.str().c_str());
        }
        
        return doc;
    }
    
    std::vector<AssetPairBase> API::parseTradableAssetPairs(uint8_t * NGA_NULLABLE jsonData) {
        if (!jsonData) {
            return {};
        }
        
        auto doc = APICreateDocument(jsonData);
        auto & result = findObject(doc, "result", emptyObjectValue);
        
        std::vector<AssetPairBase> res;
        res.reserve(result.MemberCount());
        
        for (auto it = result.MemberBegin(); it != result.MemberEnd(); ++it) {
            const char * name = it->name.GetString();
            if (name && it->value.IsObject()) {
                AssetPairBase pair;
                pair.name = name;
                pair.base = findCString(it->value, "base", emptyCString);
                pair.pairDecimals = findNumber<unsigned>(it->value, "pair_decimals", 0);
                pair.lotDecimals = findNumber<unsigned>(it->value, "lot_decimals", 0);
                pair.status = AssetPairStatusFromKey(findCString(it->value, "status", emptyCString));
                res.emplace_back(std::move(pair));
            }
        }
        
        return res;
    }
    
    std::vector<Order> API::parseOrders(uint8_t * NGA_NULLABLE jsonData, const char * NGA_NONNULL orderStatus) {
        if (!jsonData) {
            return {};
        }
        
        auto doc = APICreateDocument(jsonData);
        auto & result = findObject(doc, "result");
        auto & orders = findObject(result, orderStatus);
        
        std::vector<Order> res;
        res.reserve(orders.MemberCount());
        
        const Decimal decZero(0);
        for (auto it = orders.MemberBegin(); it != orders.MemberEnd(); ++it) {
            const char * txId = it->name.GetString();
            if (txId && it->value.IsObject()) {
                Order order;
                order.status = OrderStatusFromKey(findCString(it->value, "status"));
                if (order.status == OrderStatus{0}) {
                    throw std::runtime_error("Unknown order status");
                }
                
                order.txId = txId;
                order.clientId = findCString(it->value, "cl_ord_id", emptyCString);
                order.openTimestamp = findNumber<double>(it->value, "opentm", -1);
                order.volume = findString<Decimal>(it->value, "vol");
                order.cost = findString<Decimal>(it->value, "cost");
                order.fee = findString<Decimal>(it->value, "fee");
                order.price = findString<Decimal>(it->value, "price");
                
                auto & descr = findObject(it->value, "descr");
                order.pair = findCString(descr, "pair");
                if (order.pair.empty()) {
                    continue;
                }
                
                order.type = OrderTypeFromKey(findCString(descr, "type"));
                if (order.type == OrderType{0}) {
                    throw std::runtime_error("Unknown order type");
                }
                
                const Decimal descrPrice = findString<Decimal>(descr, "price");
                if ((order.price <= decZero) && (descrPrice > decZero)) {
                    order.price = descrPrice;
                }
                
                res.emplace_back(std::move(order));
            }
        }
        
        return res;
    }
    
    AskBid API::parseBestAskBid(uint8_t * NGA_NULLABLE jsonData, const char * aPair) {
        AskBid res(-1, -1);
        if (!jsonData) {
            return res;
        }
        
        auto doc = APICreateDocument(jsonData);
        auto & result = findObject(doc, "result");
        auto & book = findObject(result, aPair);
        auto & asks = findArray(book, "asks");
        auto & bids = findArray(book, "bids");
        
        // book object of objects of arrays of arrays of strings ...
        if (asks.Size() && bids.Size()) {
            auto & askArr = asks[0], & bidArr = bids[0];
            if (askArr.IsArray() && bidArr.IsArray()) {
                auto ask0 = askArr.GetArray(), bid0 = bidArr.GetArray();
                if (ask0.Size() && bid0.Size()) {
                    auto & ask = ask0[0], & bid = bid0[0];
                    if (ask.IsString() && bid.IsString()) {
                        if (ask.GetStringLength()) {
                            res.first = Decimal(ask.GetString());
                        }
                        if (bid.GetStringLength()) {
                            res.second = Decimal(bid.GetString());
                        }
                    }
                }
            }
        }
        
        return res;
    }
    
    ///@link https://docs.kraken.com/api/docs/rest-api/add-order
    String API::parseAddOrderTransactionId(uint8_t * NGA_NULLABLE jsonData) {
        if (!jsonData) {
            return {};
        }
        
        auto doc = APICreateDocument(jsonData);
        auto & result = findObject(doc, "result", emptyObjectValue);
        
        auto res = findString<String>(result, "txid", emptyCString);
        if (!res.empty()) {
            return res;
        }
        
        auto & txIdsArray = findArray(result, "txid", emptyArrayValue);
        if (txIdsArray.Size() == 1) {
            auto & txIdObject = txIdsArray[0];
            if (txIdObject.IsString()) {
                res = txIdObject.GetString() ?: emptyCString;
            }
        }
        
        return res;
    }
    
    String API::parseAmendOrderId(uint8_t * NGA_NULLABLE jsonData) {
        if (!jsonData) {
            return {};
        }
        
        auto doc = APICreateDocument(jsonData);
        auto & result = findObject(doc, "result", emptyObjectValue);
        
        return findString<String>(result, "amend_id", emptyCString);
    }
    
    ///@link https://docs.kraken.com/api/docs/rest-api/add-order
    std::array<char, 24> API::generateClientOrderId() noexcept {
        std::array<char, 24> res;
        char * buff = res.data();
        *buff = randomInRange<char>('G', 'Z'); // Starting with non HEX char
        for (size_t i = 0; i < 3; i++) {
            buff++;
            size_t n = i ? 5 : 4;
            for (size_t j = 0; j < n; j++) {
                if (randomInRange<size_t>(0, 1)) {
                    *buff++ = randomInRange<char>('A', 'Z');
                } else {
                    *buff++ = randomInRange<char>('0', '9');
                }
            }
            *buff = '-';
        }
        *buff = 0;
        return res;
    }
    
} // namespace kraken
} // namespace nga
