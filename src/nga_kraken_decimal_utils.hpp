/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_DECIMAL_UTILS_HPP__
#define __NGA_KRAKEN_DECIMAL_UTILS_HPP__ 1

#include <stdexcept>
#include <cstring>
#include <cstdio>

#include <boost/decimal.hpp>

namespace nga {
namespace kraken {
    
    static constexpr const size_t maxDecimalCStringLen = 256;
    
    inline void decimalToCString(const boost::decimal::decimal64_t & decNum,
                                 char * cStr,
                                 const size_t cStrSize = maxDecimalCStringLen,
                                 const int precision = 32) {
        ::memset(cStr, 0, cStrSize);
        auto res = boost::decimal::to_chars(cStr, cStr + cStrSize, decNum, std::chars_format::general, precision);
        if (res.ec != std::errc{}) {
            char reason[128];
            ::snprintf(reason, 128, "Decimal to string convertion error: %i", static_cast<int>(res.ec));
            throw std::runtime_error(reason);
        }
        *res.ptr = 0;
    }
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_DECIMAL_UTILS_HPP__
