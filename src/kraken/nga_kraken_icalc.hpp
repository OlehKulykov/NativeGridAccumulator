/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_ICALC_HPP__
#define __NGA_KRAKEN_ICALC_HPP__ 1

#include <memory>

#include "../core/nga_trio.hpp"
#include "nga_kraken_types.hpp"

namespace nga {
namespace kraken {
    
    struct Order;
    struct OrderProfit;
    typedef Trio<Decimal, Decimal, Decimal> NextInfo;
    
    class NGA_CPP_CLASS_API ICalc {
    public:
        virtual OrderProfit profit(const Order &, const Order &) const noexcept = 0;
        
        virtual OrderProfit expectedProfit(const Order &, const Order &) const noexcept = 0;
        
        virtual NextInfo nextInfo(const Decimal, const Decimal, const Decimal, const Decimal) const noexcept = 0;
        
        virtual ~ICalc() noexcept = default;
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_ICALC_HPP__
