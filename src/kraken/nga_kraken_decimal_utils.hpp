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
#include <format>
#include <cstring>
#include <cstdio>
#include <cerrno>

#include "nga_kraken_types.hpp"

#include <boost/decimal.hpp>

namespace nga {
namespace kraken {
    
    constexpr const size_t maxDecimalCStringLen = 256;
    
    inline void decimalToCString(const Decimal decNum,
                                 char * cStr,
                                 const size_t cStrSize = maxDecimalCStringLen,
                                 const int precision = 32) {
        ::memset(cStr, 0, cStrSize);
        auto convRes = boost::decimal::to_chars(cStr, cStr + cStrSize, decNum, std::chars_format::general, precision);
        if (convRes.ec != std::errc{}) {
            throw std::runtime_error(std::format("Decimal to string convertion error: {}, errno: {} ({})", static_cast<int>(convRes.ec), errno, ::strerror(errno)));
        }
        *convRes.ptr = 0;
    }
    
    template<class T = String>
    inline T decimalToString(const Decimal decNum, const int precision = 32) {
        char tmpCStrBuff[maxDecimalCStringLen];
        decimalToCString(decNum, static_cast<char *>(tmpCStrBuff), maxDecimalCStringLen, precision);
        return T(static_cast<const char *>(tmpCStrBuff));
    }
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_DECIMAL_UTILS_HPP__
