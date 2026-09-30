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

#include "../nga_types.hpp"

#include "nga_kraken_types.hpp"

namespace nga {
namespace kraken {
    
    struct NGA_CPP_CLASS_API OrderProfit final {
        Decimal volume{0};
        Decimal cost{0};
        OrderType from{0};
        OrderType to{0};
        
        String description() const;
        
        OrderProfit() noexcept = default;
        
        template<typename T>
        inline friend StringStream & operator << (StringStream & os, const OrderProfit & profit) {
            os << profit.description();
            return os;
        }
    };

} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_ORDER_PROFIT_HPP__
