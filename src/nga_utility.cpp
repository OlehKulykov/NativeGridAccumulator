/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <memory>
#include <algorithm>
#include <utility>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>

#include "nga_types.hpp"
#include "kraken/nga_kraken_api.hpp"
#include "core/nga_version.h"

static void printHelp(void) noexcept {
    std::flush(std::cout) << PROJECT_NAME << " utility, " << nga_version_string() << ", " << nga_build_string() << std::endl;
    std::cout << "-h, --help    Display this help text and exit" << std::endl;
    std::cout << "--kraken      Use Kraken service, default" << std::endl;
    std::cout << "--asset-pairs Print tradable asset pairs. Public endpoint" << std::endl;
    std::cout << "--filter      Filter string, case insensitive" << std::endl;
}

static void printAssetPairs(const nga::String & filter) {
    using namespace nga;
    
    
}

int main(int argc, const char * argv[]) {
    using namespace nga;
    
    printHelp();
    
    bool isKraken = false;
    bool isAssetPairs = false;
    String filter;
    
    for (int i = 0; i < argc; i++) {
        const char * arg = argv[i];
        if (!arg) continue;
        
        if ((::strcmp(arg, "--help") == 0) || (::strcmp(arg, "-h") == 0)) {
            printHelp();
            return EXIT_SUCCESS;
        } else if (::strcmp(arg, "--kraken") == 0) {
            isKraken = true;
        } else if (::strcmp(arg, "--asset-pairs") == 0) {
            isAssetPairs = true;
        } else if ((::strcmp(arg, "--filter") == 0) && ((i + 1) < argc)) {
            filter = argv[++i];
        }
    }
    
    if (isAssetPairs) {
        printAssetPairs(filter);
    }
    
    return EXIT_SUCCESS;
}
