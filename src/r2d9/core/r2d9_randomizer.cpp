/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <mutex>
#include <random>
#include <cstdlib>
#include <ctime>

#define __R2D9_FILE__ "Randomizer"

#include "r2d9_randomizer.hpp"

#if !defined(RAND_MAX)
#  define RAND_MAX 0x7fffffff
#endif

namespace r2d9 {
    
    void Randomizer::dummyLoop() {
        const size_t count = get<size_t>(2, 4);
        size_t res = 0;
        for (size_t i = 0; i < count; i++) {
            res += get<size_t>(1, 10);
        }
        (void)res;
    }
    
    Randomizer::Randomizer() :
        _rd(),
        _mt( _rd() ),
        _dis(0, RAND_MAX) {
            
    }
    
} // namespace r2d9
