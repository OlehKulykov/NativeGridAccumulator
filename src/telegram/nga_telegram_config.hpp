/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_TELEGRAM_CONFIG_HPP__
#define __NGA_TELEGRAM_CONFIG_HPP__ 1

#include <string>

#include "../nga_types.hpp"

namespace nga {
namespace telegram {
    
    struct NGA_CPP_CLASS_API Config final {
        String apiKey;
        String chatId;
    };
    
} // namespace telegram
} // namespace nga

#endif //!__NGA_TELEGRAM_CONFIG_HPP__
