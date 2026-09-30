/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <utility>
#include <limits>

#include "nga_kraken_order_profit.hpp"
#include "nga_kraken_order.hpp"
#include "nga_kraken_decimal_utils.hpp"

#include "nga_kraken_calc.hpp"

namespace nga {
namespace kraken {
    
    OrderProfit Calc::profit(const Order & order, const Order & parent) const noexcept {
        const Decimal decZero(0);
        OrderProfit res;
        
        const bool isValid = (order.type != OrderType{0}) && (order.volume > decZero) && (order.cost > decZero) && (order.fee >= decZero);
        if (!isValid) {
            return res;
        }
        
        const bool isParentValid = (parent.volume > decZero) && (parent.cost > decZero) && (parent.fee >= decZero);
        if (!isParentValid) {
            return res;
        }
        
        switch (order.type) {
            case OrderType::buy:
                res.from = OrderType::sell;
                res.to = order.type;
                res.volume = order.volume - parent.volume;
                res.cost = (parent.cost - parent.fee) - (order.cost + order.fee); // buy: +fee, sell: -fee
                break;
                
            case OrderType::sell:
                res.from = OrderType::buy;
                res.to = order.type;
                res.volume = parent.volume - order.volume;
                res.cost = (order.cost - order.fee) - (parent.cost + parent.fee); // buy: +fee, sell: -fee
                break;
                
            default:
                break;
        }
        return res;
    }
    
    OrderProfit Calc::expectedProfit(const Order & order, const Order & parent) const noexcept {
        const Decimal decZero(0);
        OrderProfit res;
        
        const bool isValid = (order.type != OrderType{0}) && (order.price > decZero) && (order.volume > decZero);
        if (!isValid) {
            return res;
        }
        
        const bool isParentValid = (parent.volume > decZero) && (parent.cost > decZero) && (parent.fee >= decZero);
        if (!isParentValid) {
            return res;
        }
        
        const Decimal orderBaseCost = order.volume * order.price;
        const Decimal orderCurrentFee = (orderBaseCost * _feePercent) / Decimal(100);
        
        switch (order.type) {
            case OrderType::buy:
                res.from = OrderType::sell;
                res.to = order.type;
                res.volume = order.volume - parent.volume;
                res.cost = (parent.cost - parent.fee) - (orderBaseCost + orderCurrentFee); // buy: +fee, sell: -fee
                break;
                
            case OrderType::sell:
                res.from = OrderType::buy;
                res.to = order.type;
                res.volume = parent.volume - order.volume;
                res.cost = (orderBaseCost - orderCurrentFee) - (parent.cost + parent.fee); // buy: +fee, sell: -fee
                break;
                
            default:
                break;
        }
        return res;
    }
    
    NextInfo Calc::nextInfo(const Decimal startIndex,
                            const Decimal targetVolume,
                            const Decimal targetCost,
                            const Decimal priceStep) const noexcept {
        const Decimal lotFactor = boost::decimal::pow(Decimal(10), _lotDecimals);
        const Decimal pairFactor = boost::decimal::pow(Decimal(10), _pairDecimals);
        
        NextInfo result; // price/index, volume, cost
        
        result.second = boost::decimal::trunc(targetVolume * lotFactor) / lotFactor;
        
        const bool isBuy = priceStep < Decimal(0);
        const Decimal percent100 = isBuy ? Decimal(100) : Decimal(-100); // buy: +fee, sell: -fee
        
        Decimal index = startIndex;
        
        do {
            index += priceStep;
            result.first = boost::decimal::trunc(index * pairFactor) / pairFactor;
            
            const Decimal baseCost = result.first * result.second;
            const Decimal currentFee = (baseCost * _feePercent) / percent100; // percent100 = 100.0 or -100.0
            const Decimal cost = baseCost + currentFee;                       // buy: +fee, sell: -fee
            
            result.third = boost::decimal::trunc(cost * pairFactor) / pairFactor;
            
        } while ( (isBuy && (result.third > targetCost)) ||
                 (!isBuy && (result.third < targetCost)) );
        
        return result;
    }
    
} // namespace kraken
} // namespace nga
