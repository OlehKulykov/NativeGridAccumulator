/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_ORDER_PROFIT_HPP__
#define __NGA_KRAKEN_ORDER_PROFIT_HPP__ 1

#include <sstream>
#include <ostream>

#include "../core/nga_crypto.hpp"

#include "nga_kraken_types.hpp"

namespace nga {
namespace kraken {
    
    struct NGA_CPP_CLASS_API OrderProfit final {
        decimal_t volume{0};
        decimal_t cost{0};
        OrderType from{0};
        OrderType to{0};
        
        crypto::ZeroFillString description() const;
        
        OrderProfit() noexcept = default;
        
        template<typename T>
        inline friend crypto::ZeroFillStringStream & operator << (crypto::ZeroFillStringStream & os, const OrderProfit & profit) {
            os << profit.description();
            return os;
        }
    };

} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_ORDER_PROFIT_HPP__
