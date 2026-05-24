/*
 * Copyright (C) 2018 - 2026 Oleh Kulykov <olehkulykov@gmail.com>
 * All Rights Reserved.
 *
 * Unauthorized copying of this file, transferring or reproduction of the
 * contents of this project, via any medium, is strictly prohibited.
 * The contents of this project are proprietary and confidential.
 */

#ifndef __NGA_VERSION_H__
#define __NGA_VERSION_H__ 1

#include <stddef.h>

#include "../nga.h"

NGA_C_API(const char * NGA_NONNULL) nga_version_string(void);

NGA_C_API(const char * NGA_NONNULL) nga_build_string(void);

#endif //!__NGA_VERSION_H__
