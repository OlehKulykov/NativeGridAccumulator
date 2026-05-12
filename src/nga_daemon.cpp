/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <memory>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cerrno>
#include <climits>

#include "nga.hpp"

#if __has_include("ngad-config.pch")
#  include "ngad-config.pch"
#endif

#include "kraken/nga_kraken_orders_db.hpp"
#include "kraken/nga_kraken_api.hpp"

int main(int argc, const char * argv[]) {
    using namespace nga::kraken;
    
    return EXIT_SUCCESS;
}
