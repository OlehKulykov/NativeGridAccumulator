/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include "nga_telegram_message_base.hpp"

namespace nga {
namespace telegram {
    
    void MessageBase::addBlock(String && block) {
        _blocks.emplace_back(std::move(block));
    }
    
    bool MessageBase::empty() const noexcept {
        return _blocks.empty();
    }
    
} // namespace telegram
} // namespace nga
