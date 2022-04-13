/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2021 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <openssl/err.h>
#include "crypto/sm2err.h"

#ifndef OPENSSL_NO_SM2

# ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA SM2_str_reasons[] = {
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_ASN1_ERROR), "asn1 error"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_BAD_SIGNATURE), "bad signature"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_BUFFER_TOO_SMALL), "buffer too small"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_DIST_ID_TOO_LARGE), "dist id too large"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_ID_NOT_SET), "id not set"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_ID_TOO_LARGE), "id too large"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_INVALID_CURVE), "invalid curve"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_INVALID_DIGEST), "invalid digest"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_INVALID_DIGEST_TYPE),
    "invalid digest type"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_INVALID_ENCODING), "invalid encoding"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_INVALID_FIELD), "invalid field"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_INVALID_PRIVATE_KEY),
    "invalid private key"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_NO_PARAMETERS_SET), "no parameters set"},
    {ERR_PACK(ERR_LIB_SM2, 0, SM2_R_USER_ID_TOO_LARGE), "user id too large"},
    {0, NULL}
};

# endif

int ossl_err_load_SM2_strings(void)
{
# ifndef OPENSSL_NO_ERR
    if (ERR_reason_error_string(SM2_str_reasons[0].error) == NULL)
        ERR_load_strings_const(SM2_str_reasons);
# endif
    return 1;
}
#else
NON_EMPTY_TRANSLATION_UNIT
#endif
