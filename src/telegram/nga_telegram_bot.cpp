/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <stdexcept>

#include "../core/nga_scope_guard.hpp"
#include "../core/nga_crypto.hpp"
#include "../core/nga_http.hpp"
#include "../core/nga_rapidjson.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/error/error.h>
#include <rapidjson/error/en.h>

#include "nga_telegram_bot.hpp"

namespace nga {
namespace telegram {
    
    template<class T>
    inline static void BotSetHost(T & url) {
        url << "http" << 's' << ':' << '/' << '/';
        url << "api" << '.';
        url << "tele" << "gram";
        url << '.' << "org";
    }
    
    String Bot::combineMessageBlocks(const std::vector<String> & blocks) {
        StringStream stream;
        for (const auto & block : blocks) {
            stream << block;
        }
        return stream.str();
    }
    
    void Bot::sendMessageJSON(const String & json) const {
        auto reusable = _reusable ? _reusable->get() : nullptr;
        ScopeGuard guard([=] {
            if (reusable) {
                crypto::zeroFill(*reusable);
            }
        });
        
        FixedStringStream<383> url;
        BotSetHost(url);
        url << '/' << "bot" << _aKey.c_str() << '/' << "send" << "Message";
        
        FixedStringStream<127> contentType, contentLen;
        contentType << http::headerContentType   << ": " << http::contentTypeAppJson;
        contentLen  << http::headerContentLength << ": " << json.size();
        
        const char * headers[2] = {
            static_cast<const char *>(contentType),
            static_cast<const char *>(contentLen)
        };
        
        auto response = POST(url, headers, 2, json.c_str(), json.size(), std::move(reusable));
        
#if defined(NGA_OUTPUT) || defined(DEBUG)
        if (response && response->data()) {
            std::flush(std::cout) << "JSON: " << reinterpret_cast<const char *>(response->data());
        }
#endif
        (void)response;
    }
    
    void Bot::sendMessageText(String && text) const {
        String json;
        
        {
            using namespace RAPIDJSON_NAMESPACE;
            
            auto txt = std::move(text);
            Document doc(kObjectType);
            auto & allocator = doc.GetAllocator();
            
            doc.AddMember("chat_id", StringRef(_chatId.c_str(), _chatId.size()), allocator);
            doc.AddMember("parse_mode", StringRef("HTML", 4), allocator);
            doc.AddMember("text", StringRef(txt.c_str(), txt.size()), allocator);
            
            StringStream stream;
            BasicOStreamWrapper<StringStream> wrapper(stream);
            Writer<BasicOStreamWrapper<StringStream> > writer(wrapper);
            if (doc.Accept(writer)) {
                json = stream.str();
            }
            if (json.empty()) {
                throw std::runtime_error("Error creating JSON message");
            }
        }
        
        sendMessageJSON(json);
    }
    
    void Bot::log(std::exception_ptr e) const {
        if (_logger) {
            _logger->log(e);
        }
    }
    
    void Bot::send(MessageBase && message) const {
        if (message.empty()) {
            return;
        }
        
        _dispatchQueue->add([self = weak_from_this(), blocks = std::move(message._blocks)] () mutable {
            auto strongSelf = self.lock();
            if (strongSelf) {
                try {
                    strongSelf->sendMessageText(combineMessageBlocks(blocks));
                } catch (...) {
                    strongSelf->log(std::current_exception()); 
                }
            }
        });
    }
    
    Bot::Bot(String && apiKey, String && chatId,
             const std::shared_ptr<DispatchQueue> & dispatchQueue,
             const std::shared_ptr<Logger> & logger,
             const std::shared_ptr<ReusableMT<DataVector> > & reusable) noexcept : std::enable_shared_from_this<Bot>(),
        _aKey(std::move(apiKey)),
        _chatId(std::move(chatId)),
        _dispatchQueue(dispatchQueue),
        _logger(logger),
        _reusable(reusable) {
        
    }
    
} // namespace telegram
} // namespace nga
