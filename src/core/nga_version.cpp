/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <cstddef>

#include "../nga.hpp"

#include "nga_version.h"

#define NGA_XSTR(s) NGA_STR(s)
#define NGA_STR(s) #s

const char * NGA_NONNULL nga_version_string(void) {
    return
#if defined(CLIENT_VERSION_MAJOR) && defined(CLIENT_VERSION_MINOR) && defined(CLIENT_VERSION_PATCH)
    NGA_XSTR(CLIENT_VERSION_MAJOR) "." NGA_XSTR(CLIENT_VERSION_MINOR) "." NGA_XSTR(CLIENT_VERSION_PATCH)
#  if defined(CLIENT_VERSION_BUILD)
    "." NGA_XSTR(CLIENT_VERSION_BUILD)
#  endif
#else
    "0.0.0"
#endif
    ;
}

const char * NGA_NONNULL nga_build_string(void) {
    return
#if defined(BUILD_TYPE_LOWER)
    BUILD_TYPE_LOWER ", "
#elif defined(BUILD_TYPE)
    BUILD_TYPE ", "
#elif defined(DEBUG)
    "debug, "
#endif
    
#if defined(CLIENT_TIMESTAMP)
    CLIENT_TIMESTAMP ", "
#elif defined(__DATE__) && defined(__TIME__)
    __DATE__ " " __TIME__ ", "
#elif defined(__DATE__)
    __DATE__ ", "
#elif defined(__TIME__)
    __TIME__ ", "
#endif
    
#if defined(__clang__)
    "clang"
#  if defined(__clang_major__) && defined(__clang_minor__) && defined(__clang_patchlevel__)
    "/" NGA_XSTR(__clang_major__) "." NGA_XSTR(__clang_minor__) "." NGA_XSTR(__clang_patchlevel__)
#  elif defined(__clang_major__) && defined(__clang_minor__)
    "/" NGA_XSTR(__clang_major__) "." NGA_XSTR(__clang_minor__)
#  elif defined(__clang_version__)
    "/" __clang_version__
#  endif
#elif defined(__GNUC__)
    "gcc/" NGA_XSTR(__GNUC__)
#  if defined(__GNUC_MINOR__)
    "." NGA_XSTR(__GNUC_MINOR__)
#  endif
#  if defined(__GNUC_PATCHLEVEL__)
    "." NGA_XSTR(__GNUC_PATCHLEVEL__)
#  endif
#else
    "compiller"
#endif

#if defined(BUILD_SYSTEM_NAME) && defined(BUILD_SYSTEM_PROCESSOR)
    ", " BUILD_SYSTEM_NAME " " BUILD_SYSTEM_PROCESSOR
#endif
    
#if defined(BUILD_GIT_BRANCH)
#  if defined(BUILD_GIT_COMMIT_SHORT)
     ", " BUILD_GIT_BRANCH "/" BUILD_GIT_COMMIT_SHORT
#  elif defined(BUILD_GIT_COMMIT_FULL)
      ", " BUILD_GIT_BRANCH "/" BUILD_GIT_COMMIT_FULL
#  elif defined(BUILD_GIT_COMMIT)
    ", " BUILD_GIT_BRANCH "/" BUILD_GIT_COMMIT
#  endif
#endif
    ;
}

#if 0
const char * NGA_NONNULL nga_version_about(void) {
    return
#if defined(PROJECT_NAME)
    PROJECT_NAME
#else
    "NGA"
#endif
    
#if defined(CLIENT_VERSION_MAJOR) && defined(CLIENT_VERSION_MINOR) && defined(CLIENT_VERSION_PATCH)
    "/" NGA_XSTR(CLIENT_VERSION_MAJOR) "." NGA_XSTR(CLIENT_VERSION_MINOR) "." NGA_XSTR(CLIENT_VERSION_PATCH)
#  if defined(CLIENT_VERSION_BUILD)
    "." NGA_XSTR(CLIENT_VERSION_BUILD)
#  endif
#else
    "/0.0.0"
#endif
    
#if defined(__clang__)
    " clang"
#  if defined(__clang_major__) && defined(__clang_minor__) && defined(__clang_patchlevel__)
     "/" NGA_XSTR(__clang_major__) "." NGA_XSTR(__clang_minor__) "." NGA_XSTR(__clang_patchlevel__)
#  elif defined(__clang_major__) && defined(__clang_minor__)
     "/" NGA_XSTR(__clang_major__) "." NGA_XSTR(__clang_minor__)
#  elif defined(__clang_version__)
     "/" __clang_version__
#  endif
#elif defined(__GNUC__)
    " gcc/" NGA_XSTR(__GNUC__)
#  if defined(__GNUC_MINOR__)
     "." NGA_XSTR(__GNUC_MINOR__)
#  endif
#  if defined(__GNUC_PATCHLEVEL__)
     "." NGA_XSTR(__GNUC_PATCHLEVEL__)
#  endif
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(TARGET_OS_MACCATALYST) && TARGET_OS_MACCATALYST
#  if defined(TARGET_OS_IOS) && TARGET_OS_IOS
    " (iOS"
#    if defined(TARGET_OS_SIMULATOR)
    " simulator"
#    endif
    " on macOS Catalyst"
#  else
    " (macOS Catalyst"
#  endif
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(TARGET_OS_IOS) && TARGET_OS_IOS
    " (iOS"
#  if defined(TARGET_OS_SIMULATOR)
    " simulator"
#  endif
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(TARGET_OS_TV) && TARGET_OS_TV
    " (tvOS"
#  if defined(TARGET_OS_SIMULATOR)
    " simulator"
#  endif
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(TARGET_OS_WATCH) && TARGET_OS_WATCH
    " (watchOS"
#   if defined(TARGET_OS_SIMULATOR)
     " simulator"
#   endif
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(TARGET_OS_VISION) && TARGET_OS_VISION
    " (visionOS"
#  if defined(TARGET_OS_SIMULATOR)
    " simulator"
#  endif
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(TARGET_OS_BRIDGE) && TARGET_OS_BRIDGE
    " (Apple bridge device"
#  if defined(TARGET_OS_SIMULATOR)
    " simulator"
#  endif
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    " (iOS variant"
#  if defined(TARGET_OS_SIMULATOR)
    " simulator"
#  endif
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(TARGET_OS_OSX) && TARGET_OS_OSX
    " (macOS"
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(TARGET_OS_MAC) && TARGET_OS_MAC
    " (macOS variant"
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(__APPLE__)
    " (Apple"
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && (defined(__ANDROID__) || defined(__ANDROID_API__))
    " (Android"
#  define NGA_OS_DETECTED 1
#endif
    
#if defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__)
#  if !defined(NGA_OS_DETECTED)
    " (BSD"
#    define NGA_OS_DETECTED 1
#  endif
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(__gnu_linux__)
    " (GNU/Linux"
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(__linux__)
    " (Linux"
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(__ros__)
    " (Akaros"
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(__native_client__)
    " (NaCL"
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(__asmjs__)
    " (AsmJS"
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && defined(__Fuchsia__)
    " (Fuschia"
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED) && (defined(__unix__) || defined(__unix) || (defined(TARGET_OS_UNIX) && TARGET_OS_UNIX))
    " (unix"
#  define NGA_OS_DETECTED 1
#endif
    
#if !defined(NGA_OS_DETECTED)
    " (os"
#  define NGA_OS_DETECTED 1
#endif

#if defined(BUILD_SYSTEM_PROCESSOR)
    " " BUILD_SYSTEM_PROCESSOR ")"
#elif (defined(__x86_64__) && (__x86_64__ > 0)) || (defined(__x86_64) && defined(__x86_64 > 0))
    " x86_64)"
#else
    " cpu)"
#endif
    
    ;
}
#endif
