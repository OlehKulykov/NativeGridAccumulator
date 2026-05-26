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
#include <cstring>
#include <iostream>
#include <sstream>

#include "kraken/nga_kraken_api.hpp"
#include "core/nga_version.h"
#include "core/nga_c_string.h"

static int printHelp(void) noexcept {
    std::flush(std::cout) << PROJECT_NAME << " utility, " << nga_version_string() << ", " << nga_build_string() << std::endl;
    std::cout << "-h, --help    Display this help text and exit" << std::endl;
    std::cout << "--kraken      Use Kraken service, default" << std::endl;
    std::cout << "--asset-pairs Print tradable asset pairs. Public endpoint" << std::endl;
    std::cout << "--filter      Filter string, case insensitive" << std::endl;
    return EXIT_SUCCESS;
}

static int printAssetPairs(const nga::String & filter) {
    using namespace nga;
    
    std::flush(std::cout) << "Receiving Kraken tradable asset pairs..." << std::endl;
    std::vector<kraken::AssetPairBase> pairs;
    try {
        pairs = kraken::API().tradableAssetPairs();
    } catch (const std::exception & exception) {
        std::flush(std::cout) << "Error: " << (exception.what() ?: "unknown error") << std::endl;
    }
    std::flush(std::cout) << "Done. Total " << pairs.size() << " pairs" << std::endl;
    std::flush(std::cout) << "Name | base | pair decimals | lot decimals | status" << std::endl;;
    const char * cFilter = filter.c_str();
    for (const auto & pair : pairs) {
        if (cFilter) {
            bool print = (pair.name.c_str() && ::strcasestr(pair.name.c_str(), cFilter));
            if (!print) {
                print = (pair.base.c_str() && ::strcasestr(pair.base.c_str(), cFilter));
            }
            if (!print) {
                continue;
            }
        }
        
        std::flush(std::cout) << pair.name << " | " << pair.base << " | " << pair.pairDecimals << " | " << pair.lotDecimals << " | " << kraken::AssetPairStatusToKey(pair.status) << std::endl;
    }
    std::flush(std::cout);
    return EXIT_SUCCESS;
}

int main(int argc, const char * argv[]) {
    using namespace nga;
    
    String filter;
    bool isKraken = false;
    bool isAssetPairs = false;
    
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
    
    if (isAssetPairs && isKraken) {
        return printAssetPairs(filter);
    }
    
    return printHelp();
}
