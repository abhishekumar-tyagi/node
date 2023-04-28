/*
 * WARNING: do not edit!
 * Generated by Makefile from include/openssl/fipskey.h.in
 *
 * Copyright 2020-2021 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef OPENSSL_FIPSKEY_H
# define OPENSSL_FIPSKEY_H
# pragma once

# ifdef  __cplusplus
extern "C" {
# endif

/*
 * The FIPS validation HMAC key, usable as an array initializer.
 */
#define FIPS_KEY_ELEMENTS \
    0xf4, 0x55, 0x66, 0x50, 0xac, 0x31, 0xd3, 0x54, 0x61, 0x61, 0x0b, 0xac, 0x4e, 0xd8, 0x1b, 0x1a, 0x18, 0x1b, 0x2d, 0x8a, 0x43, 0xea, 0x28, 0x54, 0xcb, 0xae, 0x22, 0xca, 0x74, 0x56, 0x08, 0x13

/*
 * The FIPS validation key, as a string.
 */
#define FIPS_KEY_STRING "f4556650ac31d35461610bac4ed81b1a181b2d8a43ea2854cbae22ca74560813"

# ifdef  __cplusplus
}
# endif

#endif
