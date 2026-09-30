/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_TELEGRAM_MESSAGE_BASE_HPP__
#define __NGA_TELEGRAM_MESSAGE_BASE_HPP__ 1

#include <memory>
#include <vector>

#include "../nga_types.hpp"

namespace nga {
namespace telegram {
    
    class NGA_CPP_CLASS_API MessageBase {
    private:
        friend class Bot;
        
        std::vector<String> _blocks;
        
    protected:
        void addBlock(String && block);
        
    public:
        bool empty() const noexcept;
        
        MessageBase() noexcept = default;
        ~MessageBase() noexcept = default;
    };
    
} // namespace telegram
} // namespace nga

#endif //!__NGA_TELEGRAM_MESSAGE_BASE_HPP__
