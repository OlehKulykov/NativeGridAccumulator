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
#include "nga_kraken_asset_pair_base.hpp"

namespace nga {
namespace kraken {
    
    class NGA_CPP_CLASS_API API final : public APIBase {
    private:
        std::shared_ptr<ReusableMT<DataVector> > _reusable;
        
        std::vector<AssetPairBase> parseTradableAssetPairs(uint8_t * NGA_NULLABLE);
        static AskBid parseBestAskBid(uint8_t * NGA_NULLABLE, const char * NGA_NONNULL);
        static std::vector<Order> parseOrders(uint8_t * NGA_NULLABLE, const char * NGA_NONNULL);
        static String parseAddOrderTransactionId(uint8_t * NGA_NULLABLE);
        static String parseAmendOrderId(uint8_t * NGA_NULLABLE);
        static std::array<char, 24> generateClientOrderId() noexcept;
        
    public:
        std::vector<AssetPairBase> tradableAssetPairs();
        
        Order fetchOpenOrder(const String & clientOrderId);
        
        std::vector<Order> fetchOpenOrders();
        
        Order fetchClosedOrder(const String & clientOrderId);
        
        std::vector<Order> fetchClosedOrders();
        
        AskBid fetchBestAskBid(const String & pair);
        
        Order addOrder(const Order & sourceOrder, const bool validateOnly);
        
        void amendOrder(const Order & sourceOrder);
        
        API(String && apiKey, String && privateKey,
            const std::shared_ptr<ReusableMT<DataVector> > & reusable = nullptr);
        
        API(const std::shared_ptr<ReusableMT<DataVector> > & reusable = nullptr);
        
        ~API() noexcept = default;
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_API_HPP__
