/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_CALC_HPP__
#define __NGA_KRAKEN_CALC_HPP__ 1

#include <memory>

#include "nga_kraken_icalc.hpp"

namespace nga {
namespace kraken {
    
    class NGA_CPP_CLASS_API Calc final : public ICalc {
    private:
        Decimal _feePercent;
        uint32_t _pairDecimals;
        uint32_t _lotDecimals;
        
    public:
        virtual OrderProfit profit(const Order &, const Order &) const noexcept override final;
        
        virtual OrderProfit expectedProfit(const Order &, const Order &) const noexcept override final;
        
        // price/index, volume, cost
        virtual NextInfo nextInfo(const Decimal, const Decimal, const Decimal, const Decimal) const noexcept override final;
        
        Calc(const Decimal feePercent, const uint32_t pairDecimals, const uint32_t lotDecimals) noexcept :
            _feePercent(feePercent),
            _pairDecimals(pairDecimals),
            _lotDecimals(lotDecimals) {
            
        }
        
        virtual ~Calc() noexcept = default;
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_CALC_HPP__
