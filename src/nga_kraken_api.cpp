/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <stdexcept>

#include <rapidjson/document.h>

#include "r2d9/core/r2d9_scope_guard.hpp"

#include "nga_kraken_api.hpp"

namespace nga {
namespace kraken {
    
    using namespace r2d9;
    
    Order API::fetchOpenOrder(const ZeroFillString & clOrdId) {
        auto reusable = _reusable ? _reusable->get() : nullptr;
        ScopeGuard guard([=] {
            if (reusable) {
                crypto::zeroFill(*reusable);
            }
        });
        
        auto json = requestPrivate("OpenOrders",
                                   std::move(reusable),
                                   "cl_ord_id", clOrdId.c_str(), nullptr);
        
        Order order;
        
        return order;
    }
    
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
        
        std::vector<Order> res;
        
        return res;
    }
    
    Order API::fetchClosedOrder(const ZeroFillString & clOrdId) {
        auto json = requestPrivate("ClosedOrders",
                                   (_reusable ? _reusable->get() : nullptr),
                                   "cl_ord_id", clOrdId.c_str(), nullptr);
        
        Order order;
        
        return order;
    }
    
    std::vector<Order> API::fetchClosedOrders() {
        auto json = requestPrivate("ClosedOrders",
                                   (_reusable ? _reusable->get() : nullptr),
                                   nullptr, nullptr);
        
        std::vector<Order> res;
        
        return res;
    }
    
    std::pair<decimal64_t, decimal64_t> API::fetchBestAskBid(const OHLCPair aPair) {
        auto json = requestPublic("Depth",
                                  (_reusable ? _reusable->get() : nullptr),
                                  "pair", OHLCPairToKey(aPair),
                                  "count", "1", nullptr);
        
        std::pair<decimal64_t, decimal64_t> res;
        
        return res;
    }
    
    API::API(const char * R2D9_NONNULL apiKey,
             const char * R2D9_NONNULL privateKey,
             const std::shared_ptr<r2d9::ReusableMT<ZeroFillDataVector> > & reusable) : APIBase(apiKey, privateKey),
        _reusable(reusable) {
        
    }
    
    API::API(const std::shared_ptr<r2d9::ReusableMT<ZeroFillDataVector> > & reusable) noexcept : APIBase(),
        _reusable(reusable) {
        
    }
    
} // namespace kraken
} // namespace nga
