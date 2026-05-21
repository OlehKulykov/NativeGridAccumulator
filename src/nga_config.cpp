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
#include <cstring>

#include "core/nga_file_utils.hpp"
#include "core/nga_rapidjson.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/error.h>
#include <rapidjson/error/en.h>

#include "nga_config.hpp"

namespace nga {
    
    std::shared_ptr<kraken::Config> ConfigParseKraken(const RAPIDJSON_NAMESPACE::Value & kraken) {
        using namespace RAPIDJSON_NAMESPACE;
        
        auto config = std::make_shared<kraken::Config>();
        
        config->apiKey = findString<String>(kraken, "api-key", emptyCString);
        config->privateKey = findString<String>(kraken, "private-key", emptyCString);
        config->ordersBD = findString<std::filesystem::path>(kraken, "orders-db-file");
        
        auto & array = findArray(kraken, "check-orders-tick-range", emptyArrayValue);
        if ((array.Size() > 1) && (array[0].IsUint()) && (array[1].IsUint())) {
            const auto elem0 = array[0].Get<unsigned>();
            const auto elem1 = array[1].Get<unsigned>();
            
            config->checkOrdersTicks.first = std::min(elem0, elem1);
            config->checkOrdersTicks.second = std::max(elem0, elem1);
        } else {
            config->checkOrdersTicks.first = config->checkOrdersTicks.second = 0;
        }
        
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
            settings.pairDecimals = findNumber<unsigned>(it->value, "pair-decimals");
            settings.lotDecimals = findNumber<unsigned>(it->value, "lot-decimals");
            settings.enabled = findBool(it->value, "enabled");
            
            config->orderSettings[pair] = std::move(settings);
        }
        
        return config;
    }
    
    std::shared_ptr<telegram::Config> ConfigParseTelegram(const RAPIDJSON_NAMESPACE::Value & telegram) {
        using namespace RAPIDJSON_NAMESPACE;
        
        auto config = std::make_shared<telegram::Config>();
        
        auto aKey = findString<String>(telegram, "api-key", emptyCString);
        auto chatId = findString<String>(telegram, "chat-id", emptyCString);
        if (aKey.size() && chatId.size()) {
            config->apiKey = std::move(aKey);
            config->chatId = std::move(chatId);
        }
        
        return config;
    }
    
    std::filesystem::path Config::path() const {
        const std::lock_guard<std::mutex> lock(_mutex);
        return _path;
    }
    
    std::shared_ptr<kraken::Config> Config::krakenConfig() const {
        const std::lock_guard<std::mutex> lock(_mutex);
        return _krakenConfig;
    }
    
    std::shared_ptr<telegram::Config> Config::telegramConfig() const {
        const std::lock_guard<std::mutex> lock(_mutex);
        return _telegramConfig;
    }
    
    std::filesystem::path Config::logFilePath() const {
        const std::lock_guard<std::mutex> lock(_mutex);
        return _logFilePath;
    }
    
    void Config::load(const char * NGA_NONNULL path) {
        using namespace RAPIDJSON_NAMESPACE;
        
        const std::lock_guard<std::mutex> lock(_mutex);
        _clear();
        
        if (!path) {
            throw std::invalid_argument("Config path is null");
        }
        
        auto content = file::readContent(path);
        
        Document doc;
        doc.ParseInsitu<kParseStopWhenDoneFlag | kParseCommentsFlag | kParseTrailingCommasFlag>(reinterpret_cast<char *>(content.data()));
        
        if (doc.HasParseError()) {
            char reason[256];
            ::snprintf(reason, 256, "Config parse error: \'%s\', offset: %" PRIu64,
                       GetParseError_En(doc.GetParseError()) ?: "Unknown",
                       static_cast<uint64_t>(doc.GetErrorOffset()));
            throw std::runtime_error(reason);
        }
        
        try {
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
        } catch (...) {
            _clear();
            throw;
        }
    }
    
    void Config::_clear() noexcept {
        _krakenConfig.reset();
        _telegramConfig.reset();
        _path.clear();
        _logFilePath.clear();
    }
    
    void Config::clearKraken() {
        const std::lock_guard<std::mutex> lock(_mutex);
        _krakenConfig.reset();
    }
    
    void Config::clearTelegram() {
        const std::lock_guard<std::mutex> lock(_mutex);
        _telegramConfig.reset();
    }
    
    void Config::clear() {
        const std::lock_guard<std::mutex> lock(_mutex);
        _clear();
    }
    
    Config::~Config() noexcept {
        _clear();
    }
    
} // namespace nga
