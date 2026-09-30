/*
 * Copyright (C) Oleh Kulykov <olehkulykov@gmail.com>. All Rights Reserved.
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Created by Oleh Kulykov <olehkulykov@gmail.com>, 2018-2025.
 */

#ifndef __NGA_HPP__
#define __NGA_HPP__ 1

#include <cstdint>      // INT_MAX|UINT_MIN, ...
#include <cinttypes>    // PRIi..., PRIu...

#include "nga.h"

#if defined(NGA_OUTPUT) || defined(DEBUG)
#  include <cstdio>   // ::printf, ::fprintf, stderr
#  include <iostream> // std::cout
#  include <sstream>  // std::stringstream
#endif

#if defined(DEBUG)
#  include <cassert>
#  if !defined(DEBUG_ASSERT)
#    define DEBUG_ASSERT(ASSERT_CONDITION) assert(ASSERT_CONDITION);
#  endif
#else
#  if !defined(DEBUG_ASSERT)
#    define DEBUG_ASSERT(ASSERT_CONDITION)
#  endif
#endif

#endif //!__NGA_HPP__
