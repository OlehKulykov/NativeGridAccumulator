/*
 * Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_KRAKEN_API_ERROR_HPP__
#define __NGA_KRAKEN_API_ERROR_HPP__ 1

#include <stdexcept>
#include <string>

#include "../nga_types.hpp"

namespace nga {
namespace kraken {
    
    enum class APIErrorSeverity : uint8_t {
        error = 1,
        warning
    };
    
    enum class APIErrorCategory : uint8_t {
        general = 1,
        auth,
        api,
        query,
        order,
        trade,
        funding,
        service
    };
    
    class NGA_CPP_CLASS_API APIError final : public std::runtime_error {
    private:
        String _what;
        String _description;
        APIErrorSeverity _severity{0};
        APIErrorCategory _category{0};
        
        void initFromStr(const char *);
        
    public:
        const char * description() const noexcept;
        
        APIErrorSeverity severity() const noexcept;
        
        APIErrorCategory category() const noexcept;
        
        virtual const char * what() const noexcept override final;
        
        APIError & operator=(APIError &&);
        APIError & operator=(const APIError &);
        
        APIError(APIError &&);
        APIError(const APIError &);
        APIError(const char *);
        
        virtual ~APIError() noexcept = default;
    };
    
} // namespace kraken
} // namespace nga

#endif //!__NGA_KRAKEN_API_ERROR_HPP__
