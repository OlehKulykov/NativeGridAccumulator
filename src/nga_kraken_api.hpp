/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_API_HPP__
#define __NGA_KRAKEN_API_HPP__ 1

#include <memory>

#include "r2d9/core/r2d9_reusable.hpp"

#include "nga_kraken_order.hpp"

#include "nga_kraken_api_base.hpp"

namespace nga {
namespace kraken {
    
    class API final : public APIBase {
    private:
        std::shared_ptr<r2d9::ReusableMT<ZeroFillDataVector> > _reusable;
        
    public:
        Order fetchOpenOrder(const ZeroFillString & clOrdId);
        
        std::vector<Order> fetchOpenOrders();
        
        Order fetchClosedOrder(const ZeroFillString & clOrdId);
        
        std::vector<Order> fetchClosedOrders();
        
        std::pair<decimal64_t, decimal64_t> fetchBestAskBid(const OHLCPair aPair);
        
        API(const char * R2D9_NONNULL apiKey,
            const char * R2D9_NONNULL privateKey,
            const std::shared_ptr<r2d9::ReusableMT<ZeroFillDataVector> > & reusable);
        
        API(const std::shared_ptr<r2d9::ReusableMT<ZeroFillDataVector> > & reusable) noexcept;
        
        ~API() noexcept = default;
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_API_HPP__
