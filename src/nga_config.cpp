/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <stdexcept>
#include <limits>
#include <format>
#include <cstring>

#include "core/nga_file_utils.hpp"
#include "core/nga_rapidjson.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/error.h>
#include <rapidjson/error/en.h>

#include "nga_config.hpp"

namespace nga {
    
    static std::pair<uint32_t, uint32_t> ConfigParseTicks(const RAPIDJSON_NAMESPACE::Value & object, const char * key) {
        using namespace RAPIDJSON_NAMESPACE;
        
        auto & array = findArray(object, key, emptyArrayValue);
        if ((array.Size() > 1) && (array[0].IsUint()) && (array[1].IsUint())) {
            const auto elem0 = array[0].Get<unsigned>();
            const auto elem1 = array[1].Get<unsigned>();
            return { std::min(elem0, elem1), std::max(elem0, elem1) };
        }
        
        return {0, 0};
    }
    
    static kraken::Config ConfigParseKraken(const RAPIDJSON_NAMESPACE::Value & kraken) {
        using namespace RAPIDJSON_NAMESPACE;
        
        kraken::Config config;
        
        config.apiKey = findString<String>(kraken, "api-key", emptyCString);
        config.privateKey = findString<String>(kraken, "private-key", emptyCString);
        config.ordersBD = findString<std::filesystem::path>(kraken, "orders-db-file");
        config.checkOrdersTicks = ConfigParseTicks(kraken, "check-orders-tick-range");
        config.updateAskBidTicks = ConfigParseTicks(kraken, "update-ask-bid-tick-range");
        config.updateOrdersInfoTicks = ConfigParseTicks(kraken, "update-orders-info-tick-range");
        
        auto & object = findObject(kraken, "order-settings", emptyObjectValue);
        for (auto it = object.MemberBegin(); it != object.MemberEnd(); ++it) {
            
            const auto pair = kraken::OHLCPairFromKey(it->name.GetString());
            if (!it->value.IsObject() || (pair == kraken::OHLCPair{0})) {
                continue;
            }
            
            kraken::OrderSettingsBase settings;
            settings.sellVolumeRate = findString<Decimal>(it->value, "sell-volume-rate");
            settings.sellCostRate = findString<Decimal>(it->value, "sell-cost-rate");
            settings.buyVolumeRate = findString<Decimal>(it->value, "buy-volume-rate");
            settings.buyCostRate = findString<Decimal>(it->value, "buy-cost-rate");
            settings.step = findString<Decimal>(it->value, "price-step");
            settings.fee = findString<Decimal>(it->value, "fee");
            settings.pairDecimals = findNumber<unsigned>(it->value, "pair-decimals");
            settings.lotDecimals = findNumber<unsigned>(it->value, "lot-decimals");
            settings.enabled = findBool(it->value, "enabled");
            
            config.orderSettings[pair] = std::move(settings);
        }
        
        return config;
    }
    
    static telegram::Config ConfigParseTelegram(const RAPIDJSON_NAMESPACE::Value & telegram) {
        using namespace RAPIDJSON_NAMESPACE;
        
        telegram::Config config;
        
        auto aKey = findString<String>(telegram, "api-key", emptyCString);
        auto chatId = findString<String>(telegram, "chat-id", emptyCString);
        if (aKey.size() && chatId.size()) {
            config.apiKey = std::move(aKey);
            config.chatId = std::move(chatId);
        }
        
        return config;
    }
    
    void Config::load(const char * NGA_NONNULL path) {
        using namespace RAPIDJSON_NAMESPACE;
        
        if (!path) {
            throw std::invalid_argument("Config: path is null");
        }
        
        auto content = file::readContent(path);
        
        Document doc;
        doc.ParseInsitu<kParseStopWhenDoneFlag | kParseCommentsFlag | kParseTrailingCommasFlag>(reinterpret_cast<char *>(content.data()));
        
        if (doc.HasParseError()) {
            throw std::runtime_error(std::format("Config: parse error: \'{}\', offset: {}", GetParseError_En(doc.GetParseError()), doc.GetErrorOffset()));
        }
        
        _path = path;
        _logFilePath = findString<std::filesystem::path>(doc, "log-file");
        
        const auto & kraken = findObject(doc, "kraken", emptyObjectValue);
        if (!kraken.ObjectEmpty()) {
            _krakenConfig = ConfigParseKraken(kraken);
        }
        
        const auto & telegram = findObject(doc, "telegram", emptyObjectValue);
        if (!telegram.ObjectEmpty()) {
            _telegramConfig = ConfigParseTelegram(telegram);
        }
    }
    
} // namespace nga
