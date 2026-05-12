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
#include <array>

#include "../core/nga_reusable.hpp"

#include "nga_kraken_order.hpp"
#include "nga_kraken_api_base.hpp"
#include "nga_kraken_api_error.hpp"

namespace nga {
namespace kraken {
    
    class API final : public APIBase {
    private:
        std::shared_ptr<ReusableMT<crypto::ZeroFillDataVector> > _reusable;
        
        static std::pair<decimal_t, decimal_t> parseBestAskBid(uint8_t * NGA_NULLABLE, const OHLCPair);
        static std::vector<Order> parseOrders(uint8_t * NGA_NULLABLE, const char * NGA_NONNULL);
        static crypto::ZeroFillString parseAddOrderTransactionId(uint8_t * NGA_NULLABLE);
        static crypto::ZeroFillString parseAmendOrderId(uint8_t * NGA_NULLABLE);
        static std::array<char, 24> generateClientOrderId() noexcept;
        
    public:
        Order fetchOpenOrder(const crypto::ZeroFillString & clientOrderId);
        
        std::vector<Order> fetchOpenOrders();
        
        Order fetchClosedOrder(const crypto::ZeroFillString & clientOrderId);
        
        std::vector<Order> fetchClosedOrders();
        
        std::pair<decimal_t, decimal_t> fetchBestAskBid(const OHLCPair pair);
        
        Order addOrder(const Order & sourceOrder, const bool validateOnly);
        
        void amendOrder(const Order & sourceOrder);
        
        API(crypto::ZeroFillString && apiKey,
            crypto::ZeroFillString && privateKey,
            const std::shared_ptr<ReusableMT<crypto::ZeroFillDataVector> > & reusable = nullptr);
        
        API(const std::shared_ptr<ReusableMT<crypto::ZeroFillDataVector> > & reusable);
        
        ~API() noexcept = default;
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_API_HPP__
