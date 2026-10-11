/*
 * Copyright (C) 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <exception>

#include "nga_demand_logger.hpp"

namespace nga {
    
    void DemandLogger::open(const char * NGA_NONNULL path) {
        if (_openCounter.fetch_add(1) == 0) {
            Logger::open(path);
            _path = path;
        }
    }
    
    void DemandLogger::close() noexcept {
        switch (_openCounter.fetch_sub(1)) {
            case 0:
                _openCounter = 0;
                break;
                
            case 1:
                _close();
                break;
                
            default:
                break;
        }
    }
    
    DemandLogger & DemandLogger::reopen() {
        if (_openCounter.fetch_add(1) == 0) {
            Logger::open(_path.c_str());
        }
        return *this;
    }
    
} // namespace nga
