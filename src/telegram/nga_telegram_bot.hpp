/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_TELEGRAM_BOT_HPP__
#define __NGA_TELEGRAM_BOT_HPP__ 1

#include <memory>
#include <mutex>

#include "../core/nga_crypto.hpp"
#include "../core/nga_reusable.hpp"
#include "../core/nga_dispatch_queue.hpp"
#include "../core/nga_curl_request.hpp"
#include "../core/nga_logger.hpp"

#include "nga_telegram_message_base.hpp"

namespace nga {
namespace telegram {
    
    class NGA_CPP_CLASS_API Bot final : protected CURLRequest {
    private:
        std::shared_ptr<ReusableMT<DataVector> > _reusable;
        String _aKey;
        String _chatId;
        
        void sendMessageJSON(const String & json) const;
        void sendMessageText(String && text) const;
        
        static String combineMessageBlocks(const std::vector<String> & blocks);
        
    public:
        void send(const MessageBase & message) const;
        
        Bot(String && apiKey, String && chatId,
            const std::shared_ptr<ReusableMT<DataVector> > & reusable = nullptr) noexcept;
    };
    
} // namespace telegram
} // namespace nga

#endif //!__NGA_TELEGRAM_BOT_HPP__
