# Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
# All Rights Reserved.
#
# Unauthorized copying of this file, transferring or reproduction of the
# contents of this project, via any medium, is strictly prohibited.
# The contents of this project are proprietary and confidential.

cmake_minimum_required(VERSION 3.31)

# https://semver.org
set(CLIENT_VERSION_MAJOR 1)
set(CLIENT_VERSION_MINOR 14)
set(CLIENT_VERSION_PATCH 2)
set(CLIENT_VENDOR "olehkulykov@gmail.com")
set(COPYRIGHT_YEAR 2026)
set(COPYRIGHT_HOLDERS "Oleh Kulykov")

set(NGA_DAEMON_NAME nga-d)

set(PACKAGE_URL "https://github.com/OlehKulykov/NativeGridAccumulator")
set(PACKAGE_BUGREPORT "https://github.com/OlehKulykov/NativeGridAccumulator")

set(PROJECT_NAME "NativeGridAccumulator")

project("${PROJECT_NAME}"
  VERSION ${CLIENT_VERSION_MAJOR}.${CLIENT_VERSION_MINOR}.${CLIENT_VERSION_PATCH}
  DESCRIPTION "Bare-Metal Automated Market Maker Accumulator"
  HOMEPAGE_URL "https://github.com/OlehKulykov/NativeGridAccumulator"
  LANGUAGES CXX C ASM
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (APPLE)
  enable_language(OBJC)
  enable_language(OBJCXX)
  if (XCODE)
    enable_language(Swift)
  endif()
  set(CMAKE_OBJCXX_STANDARD 17)
endif()

include(CheckLibraryExists)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckSymbolExists)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(ExternalProject)
include(FindThreads)
include(FetchContent)
if (APPLE)
  include(CheckOBJCCompilerFlag)
endif()

# ------ Options, consts ------
option(OPT_DEPS_PATH "Deps path")
option(OPT_BUILD_NUM "CI build number" 0)
option(OPT_OUTPUT "Enable console output: std::cout, printf, etc." OFF)
option(OPT_WARNINGS_CXX "Enable C++ warnings." OFF)

if (OPT_BUILD_NUM)
  set(CLIENT_VERSION_BUILD ${OPT_BUILD_NUM})
endif()

if(CMAKE_BUILD_TYPE)
  string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)
else()
  set(CMAKE_BUILD_TYPE_LOWER "")
endif()
string(TOLOWER ${CMAKE_GENERATOR} CMAKE_GENERATOR_LOWER)

if (CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
  string(TIMESTAMP CLIENT_TIMESTAMP "%Y-%m-%d %H:%M:%S.%f" UTC)
else()
  string(TIMESTAMP CLIENT_TIMESTAMP "%Y-%m-%d 00:00:00.00" UTC)
  add_definitions(-DDEBUG=1)
  message("Added definition: DEBUG=1")
endif()

string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_LOWER)
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} CMAKE_SYSTEM_PROCESSOR_LOWER)

execute_process(COMMAND git rev-parse --abbrev-ref HEAD
  OUTPUT_VARIABLE BUILD_GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

execute_process(COMMAND git rev-parse --short HEAD
  OUTPUT_VARIABLE BUILD_GIT_COMMIT_SHORT
  OUTPUT_STRIP_TRAILING_WHITESPACE
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

execute_process(COMMAND git rev-parse HEAD
  OUTPUT_VARIABLE BUILD_GIT_COMMIT_FULL
  OUTPUT_STRIP_TRAILING_WHITESPACE
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

message("-- PROJECT_NAME:             ${PROJECT_NAME}")
message("-- BUILD_GIT_BRANCH:         ${BUILD_GIT_BRANCH}")
message("-- BUILD_GIT_COMMIT_SHORT:   ${BUILD_GIT_COMMIT_SHORT}")
message("-- BUILD_GIT_COMMIT_FULL:    ${BUILD_GIT_COMMIT_FULL}")
message("-- CMAKE_SYSTEM_NAME:        ${CMAKE_SYSTEM_NAME}")
message("-- CMAKE_SYSTEM_PROCESSOR:   ${CMAKE_SYSTEM_PROCESSOR}")
message("-- PROJECT_BINARY_DIR:       ${PROJECT_BINARY_DIR}")
message("-- CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message("-- CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message("-- CMAKE_GENERATOR_LOWER:    ${CMAKE_GENERATOR_LOWER}")
message("-- CMAKE_BUILD_TYPE_LOWER:   ${CMAKE_BUILD_TYPE_LOWER}")
message("-- CLIENT_VERSION_BUILD:     ${CLIENT_VERSION_BUILD}")
message("-- CLIENT_TIMESTAMP:         ${CLIENT_TIMESTAMP}")

# ------ Definitions ------
if(CMAKE_GENERATOR_LOWER MATCHES "xcode")
  add_definitions(-DXCODE=1)
  message("Added definition: XCODE=1")
endif()

# C with -fPIC
check_c_compiler_flag("-fPIC" WITH_FPIC_C)
if(WITH_FPIC_C)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

# CXX with -fPIC
check_cxx_compiler_flag("-fPIC" WITH_FPIC_CXX)
if(WITH_FPIC_CXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# CXX with -fno-rtti
check_cxx_compiler_flag("-fno-rtti" WITH_FNO_RTTI_CXX)

if (APPLE)
  check_objc_compiler_flag("-fobjc-arc" WITH_FOBJC_ARC)
  if (WITH_FOBJC_ARC)
    set(CMAKE_OBJC_FLAGS "${CMAKE_OBJC_FLAGS} -fobjc-arc")
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")
  endif()
endif()

if(OPT_WARNINGS_CXX)
  check_cxx_compiler_flag("-Wall" WITH_WALL_CXX)
  if(WITH_WALL_CXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  endif()

  check_cxx_compiler_flag("-Wextra" WITH_WEXTRA_CXX)
  if(WITH_WEXTRA_CXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
  endif()

  check_cxx_compiler_flag("-Wpedantic" WITH_WPEDANTIC_CXX)
  if(WITH_WPEDANTIC_CXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic")
  endif()
endif()

# ------ Find package ------
if (OPT_DEPS_PATH)
  set(TMP_STRING "${OPT_DEPS_PATH}")
else()
  set(TMP_STRING "${PROJECT_BINARY_DIR}/../_deps")
endif()
set(TMP_STRING "${TMP_STRING}/${CMAKE_BUILD_TYPE_LOWER}_${CMAKE_SYSTEM_NAME_LOWER}_${CMAKE_SYSTEM_PROCESSOR_LOWER}")
set(FETCHCONTENT_BASE_DIR ${TMP_STRING})
unset(TMP_STRING)
message("-- FETCHCONTENT_BASE_DIR:    ${FETCHCONTENT_BASE_DIR}")

# Threads
if (UNIX)
  set(THREADS_PREFER_PTHREAD_FLAG 1)
endif()
find_package(Threads REQUIRED)

# libuv
include(FindPackageHandleStandardArgs)
set(LIBUV_NAMES libuv uv)
find_package(PkgConfig)
pkg_check_modules(PC_LIBUV QUIET ${LIBUV_NAMES})
find_path(LIBUV_INCLUDE_DIR uv.h HINTS ${PC_LIBUV_INCLUDEDIR} ${PC_LIBUV_INCLUDE_DIRS})
find_library(LIBUV_LIBRARIES NAMES ${LIBUV_NAMES} HINTS ${PC_LIBUV_LIBDIR} ${PC_LIBUV_LIBRARY_DIRS})
find_package_handle_standard_args(libuv DEFAULT_MSG LIBUV_LIBRARIES LIBUV_INCLUDE_DIR)
if (NOT libuv_FOUND)
  set(ENABLE_CLANG_TIDY 0)
  set(BUILD_SHARED_LIBS 1)
  message("-- Fetching libuv 1.52.1")
  FetchContent_Declare(libuv
    GIT_REPOSITORY https://github.com/libuv/libuv.git
    GIT_TAG        1cfa32ff59c076ffb6ed735bbc8c18361558661f  # Release: v1.52.1: 2026.03.06
    SYSTEM
    OVERRIDE_FIND_PACKAGE
  )
  FetchContent_MakeAvailable(libuv)
  find_package(libuv 1.52.1 EXACT REQUIRED)
  if (NOT TARGET libuv::libuv)
    message(FATAL_ERROR "Required libuv target not found.")
  endif()
else()
  message("libuv include dir: ${LIBUV_INCLUDE_DIR}")
  message("libuv libraries: ${LIBUV_LIBRARIES}")
endif()

# OpenSSL
include(FindOpenSSL)
set(OPENSSL_USE_STATIC_LIBS 1)
find_package(OpenSSL REQUIRED)

# cURL
include(FindCURL)
find_package(CURL)
message("cURL include dir: ${CURL_INCLUDE_DIRS}")
message("cURL libraries: ${CURL_LIBRARIES}")

# SQLite3
include(FindSQLite3)
find_package(SQLite3)
if (NOT SQLite3_FOUND)
  message("-- Fetching SQLite amalgamation 3.53.1")
  FetchContent_Declare(sqlite
    URL      https://sqlite.org/2026/sqlite-autoconf-3530100.tar.gz
    URL_HASH SHA3_256=36ca143645cf76997d07b66e9244c636b8ccdec64a1d50558259c4e415e6558b # Release: amalgamation version 3.53.1
  )
  FetchContent_MakeAvailable(sqlite)
  
  set(sqlite_INCLUDE_DIR "${sqlite_BINARY_DIR}/include")
  
  file(MAKE_DIRECTORY "${sqlite_INCLUDE_DIR}")
  file(COPY_FILE ${sqlite_SOURCE_DIR}/sqlite3.h ${sqlite_INCLUDE_DIR}/sqlite3.h)
  file(COPY_FILE ${sqlite_SOURCE_DIR}/sqlite3ext.h ${sqlite_INCLUDE_DIR}/sqlite3ext.h)
  
  add_library(sqlite_amalgamation STATIC
     ${sqlite_INCLUDE_DIR}/sqlite3.h
     ${sqlite_INCLUDE_DIR}/sqlite3ext.h
     ${sqlite_SOURCE_DIR}/sqlite3.c
  )
  
  set_target_properties(sqlite_amalgamation PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    INCLUDE_DIRECTORIES "${sqlite_INCLUDE_DIR}"
    INTERFACE_INCLUDE_DIRECTORIES "${sqlite_INCLUDE_DIR}"
  )
  
  set(SQLite3_FOUND TRUE) # TRUE from original CMake's module FindSQLite3.cmake
  set(SQLite3_INCLUDE_DIR  ${sqlite_INCLUDE_DIR})
  set(SQLite3_INCLUDE_DIRS ${sqlite_INCLUDE_DIR})
  set(SQLite3_VERSION "3.51.1")
  if (NOT TARGET SQLite::SQLite3)
    add_library(SQLite::SQLite3 ALIAS sqlite_amalgamation)
  endif()
endif()

# Boost
set(BOOST_INCLUDE_LIBRARIES decimal)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(boost_decimal
  GIT_REPOSITORY https://github.com/boostorg/decimal.git
  GIT_TAG        59d79cbe1edde0428606187cdca1477315a55f37 # Release:  boost-1.91.0
  SYSTEM
  OVERRIDE_FIND_PACKAGE
)
set(Boost_USE_DEBUG_LIBS OFF CACHE BOOL "")
set(Boost_USE_RELEASE_LIBS ON CACHE BOOL "")
set(Boost_USE_MULTITHREADED ON CACHE BOOL "")
set(BOOST_ENABLE_PYTHON OFF CACHE BOOL "")
message("-- Found Boost include: ${BOOST_INCLUDE_LIBRARIES}")
FetchContent_MakeAvailable(boost_decimal)
find_package(boost_decimal 1.91.0 EXACT REQUIRED)

# RapidJSON
FetchContent_Declare(RapidJSON
  GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
  GIT_TAG        24b5e7a8b27f42fa16b96fc70aade9106cf7102f  # Branch: master
  SYSTEM
  OVERRIDE_FIND_PACKAGE
)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_CXX11 ON CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_CXX17 ON CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_CXX20 ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(RapidJSON)
find_package(RapidJSON 1.1.0 EXACT REQUIRED)
if (NOT RapidJSON_INCLUDE_DIR)
  set(RapidJSON_INCLUDE_DIR "${RapidJSON_SOURCE_DIR}/include")
endif()

# --- Definitions -----
if(NGA_TEST)
  add_definitions(-DNGA_TEST=1)
  message("Added definition: NGA_TEST=1")
endif()

if(OPT_OUTPUT)
  add_definitions(-DNGA_OUTPUT=1)
  message("Added definition: NGA_OUTPUT=1")
endif()

message("")

# ----- Configuration -----
set(NGA_CONFIGURE_BODY "
#if defined(__cplusplus)
#  include <cstdlib>
#else
#  include <stdlib.h>
#endif

#define PROJECT_NAME \"@PROJECT_NAME@\"
#define BUILD_GIT_BRANCH \"@BUILD_GIT_BRANCH@\"
#define BUILD_GIT_COMMIT_SHORT \"@BUILD_GIT_COMMIT_SHORT@\"
#define BUILD_GIT_COMMIT_FULL \"@BUILD_GIT_COMMIT_FULL@\"
#cmakedefine BUILD_GIT_TAG \"@BUILD_GIT_TAG@\"
#define BUILD_TYPE \"@CMAKE_BUILD_TYPE@\"
#define BUILD_TYPE_LOWER \"@CMAKE_BUILD_TYPE_LOWER@\"
#define BUILD_SYSTEM_NAME \"@CMAKE_SYSTEM_NAME@\"
#define BUILD_SYSTEM_PROCESSOR \"@CMAKE_SYSTEM_PROCESSOR@\"
#define CLIENT_VERSION_MAJOR @CLIENT_VERSION_MAJOR@
#define CLIENT_VERSION_MINOR @CLIENT_VERSION_MINOR@
#define CLIENT_VERSION_PATCH @CLIENT_VERSION_PATCH@
#cmakedefine CLIENT_VERSION_BUILD @CLIENT_VERSION_BUILD@
#define CLIENT_VENDOR \"@CLIENT_VENDOR@\"
#define CLIENT_TIMESTAMP \"@CLIENT_TIMESTAMP@\"
#define COPYRIGHT_HOLDERS \"@COPYRIGHT_HOLDERS@\"
#define COPYRIGHT_YEAR @COPYRIGHT_YEAR@
")

file(CONFIGURE
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${NGA_DAEMON_NAME}-config.pch"
  CONTENT "
#ifndef __NGA_DAEMON_CONFIG_PCH__
#define __NGA_DAEMON_CONFIG_PCH__ 1
${NGA_CONFIGURE_BODY}
#endif
")

# ---- Include ----
include_directories(${PROJECT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ---- Sources & headers ----
set(NGA_CORE_HEADERS
  src/core/nga_c_string.h
  src/core/nga_crypto.hpp
  src/core/nga_curl_request.hpp
  src/core/nga_dispatch_queue.hpp
  src/core/nga_file_utils.hpp
  src/core/nga_fixed_string_stream.hpp
  src/core/nga_http.hpp
  src/core/nga_logger.hpp
  src/core/nga_noexcept_unique_lock.hpp
  src/core/nga_pair.hpp
  src/core/nga_randomizer.hpp
  src/core/nga_rapidjson.hpp
  src/core/nga_raw_heap_memory.hpp
  src/core/nga_reusable.hpp
  src/core/nga_ring_queue.hpp
  src/core/nga_scope_guard.hpp
  src/core/nga_semi_fixed_array.hpp
  src/core/nga_sqlite_db.hpp
  src/core/nga_time_utils.hpp
  src/core/nga_trio.hpp
  src/core/nga_utils.hpp
  src/core/nga_version.h
  src/nga_types.hpp
  src/nga.h
  src/nga.hpp
)

set(NGA_CORE_SOURCES
  src/core/nga_c_string.c
  src/core/nga_crypto.cpp
  src/core/nga_curl_request.cpp
  src/core/nga_dispatch_queue.cpp
  src/core/nga_file_utils.cpp
  src/core/nga_http.cpp
  src/core/nga_logger.cpp
  src/core/nga_rapidjson.cpp
  src/core/nga_sqlite_db.cpp
  src/core/nga_version.cpp
)

set(NGA_KRAKEN_HEADERS
  src/kraken/nga_kraken_api.hpp
  src/kraken/nga_kraken_api_base.hpp
  src/kraken/nga_kraken_api_error.hpp
  src/kraken/nga_kraken_app.hpp
  src/kraken/nga_kraken_calc.hpp
  src/kraken/nga_kraken_config.hpp
  src/kraken/nga_kraken_db_order.hpp
  src/kraken/nga_kraken_decimal_utils.hpp
  src/kraken/nga_kraken_dummy_calc.hpp
  src/kraken/nga_kraken_icalc.hpp
  src/kraken/nga_kraken_order.hpp
  src/kraken/nga_kraken_order_profit.hpp
  src/kraken/nga_kraken_order_settings_base.hpp
  src/kraken/nga_kraken_orders_db.hpp
  src/kraken/nga_kraken_telegram_message.hpp
  src/kraken/nga_kraken_types.hpp
)

set(NGA_KRAKEN_SOURCES
  src/kraken/nga_kraken_api.cpp
  src/kraken/nga_kraken_api_base.cpp
  src/kraken/nga_kraken_api_error.cpp
  src/kraken/nga_kraken_app.cpp
  src/kraken/nga_kraken_calc.cpp
  src/kraken/nga_kraken_db_order.cpp
  src/kraken/nga_kraken_order.cpp
  src/kraken/nga_kraken_order_profit.cpp
  src/kraken/nga_kraken_orders_db.cpp
  src/kraken/nga_kraken_telegram_message.cpp
  src/kraken/nga_kraken_types.cpp
)

set(NGA_TELEGRAM_HEADERS
  src/telegram/nga_telegram_bot.hpp
  src/telegram/nga_telegram_config.hpp
  src/telegram/nga_telegram_message_base.hpp
)

set(NGA_TELEGRAM_SOURCES
  src/telegram/nga_telegram_bot.cpp
  src/telegram/nga_telegram_message_base.cpp
)

# ---- Groups ----
source_group("core"
  FILES
  src/core/nga_c_string.c
  src/core/nga_c_string.h
  src/core/nga_crypto.cpp
  src/core/nga_crypto.hpp
  src/core/nga_curl_request.cpp
  src/core/nga_curl_request.hpp
  src/core/nga_dispatch_queue.cpp
  src/core/nga_dispatch_queue.hpp
  src/core/nga_file_utils.cpp
  src/core/nga_file_utils.hpp
  src/core/nga_fixed_string_stream.hpp
  src/core/nga_http.cpp
  src/core/nga_http.hpp
  src/core/nga_logger.cpp
  src/core/nga_logger.hpp
  src/core/nga_noexcept_unique_lock.hpp
  src/core/nga_pair.hpp
  src/core/nga_randomizer.hpp
  src/core/nga_rapidjson.cpp
  src/core/nga_rapidjson.hpp
  src/core/nga_raw_heap_memory.hpp
  src/core/nga_reusable.hpp
  src/core/nga_ring_queue.hpp
  src/core/nga_scope_guard.hpp
  src/core/nga_semi_fixed_array.hpp
  src/core/nga_sqlite_db.cpp
  src/core/nga_sqlite_db.hpp
  src/core/nga_time_utils.hpp
  src/core/nga_trio.hpp
  src/core/nga_utils.hpp
  src/core/nga_version.cpp
  src/core/nga_version.h
  src/nga_types.hpp
  src/nga.h
  src/nga.hpp
)

source_group("kraken"
  FILES
  src/kraken/nga_kraken_api.cpp
  src/kraken/nga_kraken_api.hpp
  src/kraken/nga_kraken_api_base.cpp
  src/kraken/nga_kraken_api_base.hpp
  src/kraken/nga_kraken_api_error.cpp
  src/kraken/nga_kraken_api_error.hpp
  src/kraken/nga_kraken_app.cpp
  src/kraken/nga_kraken_app.hpp
  src/kraken/nga_kraken_calc.cpp
  src/kraken/nga_kraken_calc.hpp
  src/kraken/nga_kraken_config.hpp
  src/kraken/nga_kraken_db_order.cpp
  src/kraken/nga_kraken_db_order.hpp
  src/kraken/nga_kraken_decimal_utils.hpp
  src/kraken/nga_kraken_dummy_calc.hpp
  src/kraken/nga_kraken_icalc.hpp
  src/kraken/nga_kraken_order.cpp
  src/kraken/nga_kraken_order.hpp
  src/kraken/nga_kraken_order_profit.cpp
  src/kraken/nga_kraken_order_profit.hpp
  src/kraken/nga_kraken_order_settings_base.hpp
  src/kraken/nga_kraken_orders_db.cpp
  src/kraken/nga_kraken_orders_db.hpp
  src/kraken/nga_kraken_telegram_message.cpp
  src/kraken/nga_kraken_telegram_message.hpp
  src/kraken/nga_kraken_types.cpp
  src/kraken/nga_kraken_types.hpp
)

source_group("telegram"
  FILES
  src/telegram/nga_telegram_bot.cpp
  src/telegram/nga_telegram_bot.hpp
  src/telegram/nga_telegram_config.hpp
  src/telegram/nga_telegram_message_base.cpp
  src/telegram/nga_telegram_message_base.hpp
)

source_group("daemon"
  FILES
  src/nga_config.cpp
  src/nga_config.hpp
  src/nga_daemon.cpp
)

# ------ Binaries, Link (in Park) ---------
add_executable(${NGA_DAEMON_NAME}
  "src/nga_daemon.cpp" "src/nga_config.cpp" "src/nga_config.hpp"
  ${NGA_CORE_HEADERS} ${NGA_CORE_SOURCES}
  ${NGA_KRAKEN_HEADERS} ${NGA_KRAKEN_SOURCES}
  ${NGA_TELEGRAM_HEADERS} ${NGA_TELEGRAM_SOURCES}
)

target_include_directories(${NGA_DAEMON_NAME} SYSTEM AFTER
  PRIVATE "${CURL_INCLUDE_DIRS}"
  PRIVATE "${RapidJSON_INCLUDE_DIR}"
  PRIVATE SQLite::SQLite3
  PRIVATE OpenSSL::Crypto
  PRIVATE Boost::decimal
)

target_link_libraries(${NGA_DAEMON_NAME}
  PRIVATE ${CURL_LIBRARIES}
  PRIVATE Threads::Threads
  PRIVATE SQLite::SQLite3
  PRIVATE OpenSSL::Crypto
  PRIVATE Boost::decimal
)

if (TARGET libuv::libuv)
  target_include_directories(${NGA_DAEMON_NAME} SYSTEM AFTER
    PRIVATE libuv::libuv
  )
  target_link_libraries(${NGA_DAEMON_NAME}
    PRIVATE libuv::libuv
  )
else()
  target_include_directories(${NGA_DAEMON_NAME} SYSTEM AFTER
    PRIVATE "${LIBUV_INCLUDE_DIR}"
  )
  target_link_libraries(${NGA_DAEMON_NAME}
    PRIVATE ${LIBUV_LIBRARIES}
  )
endif()

if(APPLE)
  target_link_libraries(${NGA_DAEMON_NAME}
    PRIVATE "-framework CoreFoundation -framework CryptoKit -framework Security"
  )
endif()

set_property(TARGET ${NGA_DAEMON_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
target_precompile_headers(${NGA_DAEMON_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/${NGA_DAEMON_NAME}-config.pch")

# ------ testing ---------
enable_testing()

include(CPack)
