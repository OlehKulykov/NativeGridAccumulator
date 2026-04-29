/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#include <stdlib.h> // malloc, free
#include <string.h> // memcpy

#include "r2d9_c_string.h"

const char * R2D9_NONNULL const emptyCString = "";

char * R2D9_NONNULL r2d9_c_str_copy_non_empty(const char * R2D9_NULLABLE cString) {
    const size_t cStringLength = cString ? strlen(cString) : 0;
    if (cStringLength > 0) {
        char * cStringCopy = (char *)malloc(cStringLength + 1);
        if (cStringCopy) {
            memcpy(cStringCopy, cString, cStringLength);
            cStringCopy[cStringLength] = 0;
            return cStringCopy;
        }
    }
    return NULL;
}

char * R2D9_NONNULL r2d9_c_str_copy_with_empty(const char * R2D9_NULLABLE cString) {
    if (cString) {
        const size_t cStringLength = strlen(cString);
        char * cStringCopy = (char *)malloc(cStringLength + 1);
        if (cStringCopy) {
            if (cStringLength > 0) {
                memcpy(cStringCopy, cString, cStringLength);
            }
            cStringCopy[cStringLength] = 0;
            return cStringCopy;
        }
    }
    return NULL;
}

char * R2D9_NONNULL r2d9_c_str_init_len(const size_t len) {
    char * cString = (char *)malloc(len + 1);
    if (cString) {
        cString[len] = 0;
    }
    return cString;
}

void r2d9_c_str_release(char * R2D9_NULLABLE cString) {
    if (cString) {
        free(cString);
    }
}

void r2d9_c_str_release_zero_fill(char * R2D9_NULLABLE cString) {
    if (cString) {
        const size_t len = strlen(cString);
        if (len) {
            memset(cString, 0, len);
        }
        free(cString);
    }
}
