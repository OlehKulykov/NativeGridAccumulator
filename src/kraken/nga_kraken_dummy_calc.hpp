/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_DUMMY_CALC_HPP__
#define __NGA_KRAKEN_DUMMY_CALC_HPP__ 1

#include <memory>

#include "nga_kraken_order_profit.hpp"
#include "nga_kraken_order.hpp"
#include "nga_kraken_icalc.hpp"

namespace nga {
namespace kraken {
    
    class NGA_CPP_CLASS_API DummyCalc final : public ICalc {
    public:
        virtual OrderProfit profit(const Order &, const Order &) const noexcept override final {
            return {};
        }
        
        virtual OrderProfit expectedProfit(const Order &, const Order &) const noexcept override final {
            return {};
        }
        
        virtual NextInfo nextInfo(const decimal_t, const decimal_t, const decimal_t, const decimal_t) const noexcept override final {
            return {};
        }
        
        DummyCalc() noexcept = default;
        virtual ~DummyCalc() noexcept = default;
    };
        
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_DUMMY_CALC_HPP__
