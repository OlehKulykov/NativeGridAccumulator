/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <memory>
#include <cstring>

#include <openssl/evp.h>
#include <openssl/params.h>
#include <openssl/buffer.h>
#include <openssl/bio.h>

#include "r2d9_crypto.hpp"

namespace r2d9 {
namespace crypto {
    
    struct BIODeleter final {
        void operator()(BIO * R2D9_NULLABLE bio) const noexcept {
            if (bio) {
                ::BIO_free_all(bio);
            }
        }
    };
    
    struct EVP_MACDeleter final {
        void operator()(EVP_MAC * R2D9_NULLABLE mac) const noexcept {
            if (mac) {
                ::EVP_MAC_free(mac);
            }
        }
    };
    
    struct EVP_MAC_CTXDeleter final {
        void operator()(EVP_MAC_CTX * R2D9_NULLABLE ctx) const noexcept {
            if (ctx) {
                ::EVP_MAC_CTX_free(ctx);
            }
        }
    };
    
    struct EVP_MD_CTXDeleter final {
        void operator()(EVP_MD_CTX * R2D9_NULLABLE ctx) const noexcept {
            if (ctx) {
                ::EVP_MD_CTX_free(ctx);
            }
        }
    };
    
    struct EVP_MDDeleter final {
        void operator()(EVP_MD * R2D9_NULLABLE md) const noexcept {
            if (md) {
                ::EVP_MD_free(md);
            }
        }
    };
    
    SHA256Buffer sha256(const void * R2D9_NONNULL data, const size_t dataSize) {
        std::unique_ptr<EVP_MD, EVP_MDDeleter> md(::EVP_MD_fetch(nullptr, "SHA256", nullptr));
        if (!md) {
            throw std::runtime_error("OpenSSL: Failed to fetch SHA256");
        }
        
        std::unique_ptr<EVP_MD_CTX, EVP_MD_CTXDeleter> ctx(::EVP_MD_CTX_new());
        if (!ctx) {
            throw std::runtime_error("OpenSSL: Failed to create MD context");
        }
        
        if (::EVP_DigestInit_ex(ctx.get(), md.get(), nullptr) != 1) {
            throw std::runtime_error("OpenSSL: Digest init failed");
        }
        
        if (::EVP_DigestUpdate(ctx.get(), data, dataSize) != 1) {
            throw std::runtime_error("OpenSSL: Digest update failed");
        }
        
        SHA256Buffer shaRes;
        unsigned int len = 0;
        if (::EVP_DigestFinal_ex(ctx.get(), shaRes.data(), &len) != 1) {
            throw std::runtime_error("OpenSSL: Digest final failed");
        }
        
        if (len != shaRes.size()) {
            throw std::runtime_error("OpenSSL: Unexpected SHA256 length");
        }
        
        return shaRes;
    }
    
    HMAC512Buffer hmacSha512(const void * R2D9_NONNULL key, const size_t keySize,
                             const void * R2D9_NONNULL data, const size_t dataSize) {
        std::unique_ptr<EVP_MAC, EVP_MACDeleter> mac(::EVP_MAC_fetch(nullptr, "HMAC", nullptr));
        if (!mac) {
            throw std::runtime_error("OpenSSL: Failed to fetch HMAC");
        }
        
        std::unique_ptr<EVP_MAC_CTX, EVP_MAC_CTXDeleter> ctx(::EVP_MAC_CTX_new(mac.get()));
        if (!ctx) {
            throw std::runtime_error("OpenSSL: Failed to create context");
        }
        
        char buff0[7]{'S','H','A','5','1','2',0};
        OSSL_PARAM params[2];
        params[0] = ::OSSL_PARAM_construct_utf8_string("digest", buff0, 0);
        params[1] = ::OSSL_PARAM_construct_end();
        
        if (::EVP_MAC_init(ctx.get(), static_cast<const unsigned char *>(key), keySize, params) != 1) {
            throw std::runtime_error("OpenSSL: MAC init failed");
        }
        
        if (::EVP_MAC_update(ctx.get(), static_cast<const unsigned char *>(data), dataSize) != 1) {
            throw std::runtime_error("OpenSSL: MAC update failed");
        }
        
        HMAC512Buffer hmacRes;
        size_t len = 0;
        if (::EVP_MAC_final(ctx.get(), hmacRes.data(), &len, hmacRes.size()) != 1) {
            throw std::runtime_error("OpenSSL: MAC final failed");
        }
        
        if (len != hmacRes.size()) {
            throw std::runtime_error("OpenSSL: Unexpected MAC length");
        }
        
        return hmacRes;
    }
    
    ZeroFillString base64Encode(const void * R2D9_NONNULL data, const size_t dataSize) {
        if (dataSize == 0) return "";
        
        std::unique_ptr<BIO, BIODeleter> b64(::BIO_new(::BIO_f_base64()));
        if (!b64) {
            throw std::runtime_error("OpenSSL: Failed to create Base64 filter");
        }
        
        std::unique_ptr<BIO, BIODeleter> mem(::BIO_new(::BIO_s_mem()));
        if (!mem) {
            throw std::runtime_error("OpenSSL: Failed to create memory sink");
        }
        
        BIO * chain = ::BIO_push(b64.get(), mem.get());
        if (!chain) {
            throw std::runtime_error("OpenSSL: Failed to chain BIOs");
        }
        
        mem.release(); // mem became b64 chain
        
        ::BIO_set_flags(chain, BIO_FLAGS_BASE64_NO_NL);
        
        if (::BIO_write(chain, data, static_cast<int>(dataSize)) <= 0) {
            throw std::runtime_error("OpenSSL: Base64 encode failed");
        }
        
        if (BIO_flush(chain) != 1) {
            throw std::runtime_error("OpenSSL: Base64 flush failed");
        }
        
        BUF_MEM * ptr = nullptr;
        ::BIO_get_mem_ptr(chain, &ptr);
        
        if (!ptr || !ptr->data) {
            throw std::runtime_error("OpenSSL: Failed to retrieve mem ptr");
        }
        
        ZeroFillString base64String(ptr->data, ptr->length);
        
        ::memset(ptr->data, 0, ptr->length);
        
        return base64String;
    }
    
    ZeroFillDataVector base64Decode(const void * R2D9_NONNULL data, const size_t dataSize) {
        if (dataSize == 0) return {};
        
        std::unique_ptr<BIO, BIODeleter> b64(::BIO_new(::BIO_f_base64()));
        if (!b64) {
            throw std::runtime_error("OpenSSL: Failed to create Base64 filter");
        }
        
        std::unique_ptr<BIO, BIODeleter> mem(::BIO_new_mem_buf(data, static_cast<int>(dataSize)));
        if (!mem) {
            throw std::runtime_error("OpenSSL: Failed to create memory source");
        }
        
        BIO * chain = ::BIO_push(b64.get(), mem.get());
        if (!chain) {
            throw std::runtime_error("OpenSSL: Failed to chain BIOs");
        }
        
        mem.release(); // mem became b64 chain
        
        ::BIO_set_flags(chain, BIO_FLAGS_BASE64_NO_NL);
        
        ZeroFillDataVector result(dataSize);
        
        const int decodedSize = ::BIO_read(chain, result.data(), static_cast<int>(dataSize));
        if (decodedSize < 0) {
            throw std::runtime_error("OpenSSL: Base64 decode failed");
        }
        
        result.resize(static_cast<size_t>(decodedSize));
        return result;
    }
    
} // namespace crypto
} // namespace r2d9
