/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_PAIR_HPP__
#define __NGA_PAIR_HPP__ 1

#include <algorithm>

#include "../nga.hpp"

namespace nga {
    
    // POD(Plain Old Data)
    template<typename T1, typename T2>
    struct PairPOD final {
        T1 first;
        T2 second;
    };
    
    // #include <type_traits>
    //static_assert(std::is_pod<PairPOD<..., ...> >::value, "POD");
    //static_assert(std::is_standard_layout<PairPOD<..., ...> >::value, "standard layout");
    //static_assert(std::is_trivial<PairPOD<..., ...> >::value, "trivial");
    
} // namespace nga

#include <utility>      // std::hash
#include <functional>   // std::equal_to

namespace std {
    
    template<typename T1, typename T2>
    struct hash<nga::PairPOD<T1, T2> > {
        size_t operator()(const nga::PairPOD<T1, T2> & pair) const {
            return (std::hash<T1>()(pair.first)) ^
                   (((std::hash<T2>()(pair.second)) << 1) >> 1);
        }
    };
    
} // namespace std

#endif //!__NGA_PAIR_HPP__
