/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __R2D9_HPP__
#define __R2D9_HPP__ 1

#include <cstdint>      // INT_MAX|UINT_MIN, ...
#include <cinttypes>    // PRIi..., PRIu...

#include "r2d9.h"

#if defined(R2D9_OUTPUT) || defined(DEBUG)
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

#endif //!__R2D9_HPP__
