/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_H__
#define __NGA_H__ 1

#include <stdint.h>

#if !defined(DEBUG) && defined(_DEBUG)
#  define DEBUG 1
#endif

#if defined(DEBUG) && defined(NDEBUG)
#  undef DEBUG
#endif

// C / C++ extern
#if defined(__cplusplus)
#  define NGA_C_EXTERN extern "C"
#  define NGA_CPP_EXTERN extern
#else
#  define NGA_C_EXTERN extern
#  define NGA_CPP_EXTERN extern "C++"
#endif

// Attribute
#if defined(__GNUC__) && (__GNUC__ >= 4)
#  define NGA_ATTRIB __attribute__((visibility("default")))
#  define NGA_ATTRIB_PRIVATE __attribute__((visibility("hidden")))
#else
#  define NGA_ATTRIB
#  define NGA_ATTRIB_PRIVATE
#endif

// Combined lib API
#define NGA_C_API(RETURN_TYPE) NGA_C_EXTERN NGA_ATTRIB RETURN_TYPE
#define NGA_C_API_PRIVATE(RETURN_TYPE) NGA_C_EXTERN NGA_ATTRIB_PRIVATE RETURN_TYPE
#define NGA_CPP_API(RETURN_TYPE) NGA_CPP_EXTERN NGA_ATTRIB RETURN_TYPE
#define NGA_CPP_API_PRIVATE(RETURN_TYPE) NGA_CPP_EXTERN NGA_ATTRIB_PRIVATE RETURN_TYPE
#define NGA_CPP_CLASS_API NGA_ATTRIB
#define NGA_CPP_CLASS_API_PRIVATE NGA_ATTRIB_PRIVATE

#if defined(__APPLE__) && defined(__clang__)
#  define NGA_NULLABLE __nullable
#  define NGA_NONNULL __nonnull
#else
#  define NGA_NULLABLE
#  define NGA_NONNULL
#endif

#if __has_attribute(sentinel)
#  define NGA_REQUIRES_LAST_NULL_ARG __attribute__((sentinel))
#else
#  define NGA_REQUIRES_LAST_NULL_ARG
#endif

#endif //!__NGA_H__
