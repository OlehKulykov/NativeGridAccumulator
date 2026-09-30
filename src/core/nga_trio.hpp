/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_TRIO_HPP__
#define __NGA_TRIO_HPP__ 1

#include <algorithm>

#include "../nga.hpp"

namespace nga {
    
    // POD(Plain Old Data)
    template<typename T1, typename T2, typename T3>
    struct TrioPOD final {
        T1 first;
        T2 second;
        T3 third;
    };
    
    //static_assert(std::is_pod<TrioPOD<..., ..., ...> >::value, "POD");
    //static_assert(std::is_standard_layout<TrioPOD<..., ..., ...> >::value, "standard layout");
    //static_assert(std::is_trivial<TrioPOD<..., ..., ...> >::value, "trivial");
    
    template<class T1, class T2, class T3>
    struct Trio final {
        T1 first;
        T2 second;
        T3 third;
        
        Trio<T1, T2, T3> & operator = (const Trio<T1, T2, T3> & trio) {
            first = trio.first;
            second = trio.second;
            third = trio.third;
            return *this;
        }
        
        Trio<T1, T2, T3> & operator = (Trio<T1, T2, T3> && trio) noexcept {
            first = static_cast<T1 &&>(trio.first);
            second = static_cast<T2 &&>(trio.second);
            third = static_cast<T3 &&>(trio.third);
            return *this;
        }
        
        bool operator != (const Trio<T1, T2, T3> & trio) const {
            return ((first != trio.first) || (second != trio.second) || (third != trio.third));
        }
        
        bool operator == (const Trio<T1, T2, T3> & trio) const {
            return ((first == trio.first) && (second == trio.second) && (third == trio.third));
        }
        
        Trio(Trio<T1, T2, T3> && trio) noexcept : first(static_cast<T1 &&>(trio.first)), second(static_cast<T2 &&>(trio.second)), third(static_cast<T3 &&>(trio.third)) { }
        Trio(const Trio<T1, T2, T3> & trio) : first(trio.first), second(trio.second), third(trio.third) { }
        Trio(const T1 & aFirst, const T2 & aSecond, const T3 & aThird) : first(aFirst), second(aSecond), third(aThird) { }
        Trio(T1 && aFirst, T2 && aSecond, T3 && aThird) noexcept : first(static_cast<T1 &&>(aFirst)), second(static_cast<T2 &&>(aSecond)), third(static_cast<T3 &&>(aThird)) { }
        Trio() = default;
    };
    
} // namespace nga

#include <utility>      // std::hash
#include <functional>   // std::equal_to

namespace std {
    
    template<typename T1, typename T2, typename T3>
    struct hash<nga::TrioPOD<T1, T2, T3> > {
        size_t operator()(const nga::TrioPOD<T1, T2, T3> & trio) const {
            return (std::hash<T1>()(trio.first)) ^
                   (((std::hash<T2>()(trio.second)) << 1) >> 1) ^
                   (std::hash<T3>()(trio.third) << 1);
        }
    };
    
    template<class T1, class T2, class T3>
    struct hash<nga::Trio<T1, T2, T3> > {
        size_t operator()(const nga::Trio<T1, T2, T3> & trio) const {
            return (std::hash<T1>()(trio.first)) ^
                   (((std::hash<T2>()(trio.second)) << 1) >> 1) ^
                   (std::hash<T3>()(trio.third) << 1);
        }
    };
    
} // namespace nga

//Number    Group name
//  3         trio
//  4        quartet
//  5        quintet
//  6        sextet
//  7        septet
//  8         octet
//  9         nonet

#endif //!__NGA_TRIO_HPP__
