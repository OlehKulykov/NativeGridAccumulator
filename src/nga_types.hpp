/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_TYPES_HPP__
#define __NGA_TYPES_HPP__ 1

#include <ctime>

namespace nga {
    
    enum class Trend : uint8_t {
        none    = 0,
        up      = 1,
        down    = 2,
        flat    = 3
    };
    
    enum TimeDuration : time_t {
        TimeDuration_1minute    = 60,
        TimeDuration_1hour      = 3600,
        TimeDuration_1day       = 86400,
        TimeDuration_1week      = 604800,
        TimeDuration_1month     = 2629743,
        TimeDuration_1year      = 31556926
    };
    
    enum class OrderStatus : uint8_t {
        open        = 1,
        closed      = 2,
        canceled    = 3,
        expired     = 4,
        pending     = 5
    };
    
    enum class OrderType : uint8_t {
        buy     = 1,
        sell    = 2
    };
    
} // namespace nga

#endif //!__NGA_TYPES_HPP__
