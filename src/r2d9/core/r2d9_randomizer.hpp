/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __R2D9_RANDOMIZER_HPP__
#define __R2D9_RANDOMIZER_HPP__ 1

#include <random>
#include <cstdlib>

#include "../r2d9.hpp"

namespace r2d9 {
    
    class Randomizer final {
    private:
        std::random_device _rd;
        mutable std::mt19937_64 _mt;
        mutable std::uniform_int_distribution<size_t> _dis;
        
    public:
        template<typename T>
        T get(const size_t lowRange, const size_t upRange) const {
            return static_cast<T>((_dis(_mt) % (upRange - lowRange + 1)) + lowRange);
        }
        
        template<typename T>
        void shuffle(T * R2D9_NONNULL refsPtr, const size_t refsCount, const size_t times) const {
            if (refsPtr && (refsCount > 1)) {
                for (size_t i = refsCount - 1; i > 0; i--) {
                    const size_t j = get<size_t>(0, i);
                    
                    const T tmpValue = refsPtr[i];
                    refsPtr[i] = refsPtr[j];
                    refsPtr[j] = tmpValue;
                }
            }
        }
        
        void dummyLoop();
        
        Randomizer();
        ~Randomizer() noexcept = default;
    };
    
    // noexcept
    template<typename T>
    T randomInRange(const uint32_t lowRange, const uint32_t upRange) noexcept {
        return static_cast<T>((arc4random() % (upRange - lowRange + 1)) + lowRange);
    }
    
    // std::random_shuffle, std::shuffle noexcept
    template<typename T>
    void randomShuffle(T * R2D9_NONNULL refsPtr, const uint32_t refsCount) noexcept {
        if (refsPtr && (refsCount > 1)) {
            for (uint32_t i = refsCount - 1; i > 0; i--) {
                const size_t j = randomInRange<uint32_t>(0, i);
                
                const T tmpValue = refsPtr[i];
                refsPtr[i] = refsPtr[j];
                refsPtr[j] = tmpValue;
            }
        }
    }
    
} // namespace r2d9

#endif //!__R2D9_RANDOMIZER_HPP__
