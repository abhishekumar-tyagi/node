/* crypto/ocsp/ocsp_err.c */
/* ====================================================================
 * Copyright (c) 1999-2006 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

/*
 * NOTE: this file was auto generated by the mkerr.pl script: any changes
 * made to it will be overwritten when the script next updates this file,
 * only reason strings will be preserved.
 */

#include <stdio.h>
#include <openssl/err.h>
#include <openssl/ocsp.h>

/* BEGIN ERROR CODES */
#ifndef OPENSSL_NO_ERR

# define ERR_FUNC(func) ERR_PACK(ERR_LIB_OCSP,func,0)
# define ERR_REASON(reason) ERR_PACK(ERR_LIB_OCSP,0,reason)

static ERR_STRING_DATA OCSP_str_functs[] = {
    {ERR_FUNC(OCSP_F_ASN1_STRING_ENCODE), "ASN1_STRING_encode"},
    {ERR_FUNC(OCSP_F_D2I_OCSP_NONCE), "D2I_OCSP_NONCE"},
    {ERR_FUNC(OCSP_F_OCSP_BASIC_ADD1_STATUS), "OCSP_basic_add1_status"},
    {ERR_FUNC(OCSP_F_OCSP_BASIC_SIGN), "OCSP_basic_sign"},
    {ERR_FUNC(OCSP_F_OCSP_BASIC_VERIFY), "OCSP_basic_verify"},
    {ERR_FUNC(OCSP_F_OCSP_CERT_ID_NEW), "OCSP_cert_id_new"},
    {ERR_FUNC(OCSP_F_OCSP_CHECK_DELEGATED), "OCSP_CHECK_DELEGATED"},
    {ERR_FUNC(OCSP_F_OCSP_CHECK_IDS), "OCSP_CHECK_IDS"},
    {ERR_FUNC(OCSP_F_OCSP_CHECK_ISSUER), "OCSP_CHECK_ISSUER"},
    {ERR_FUNC(OCSP_F_OCSP_CHECK_VALIDITY), "OCSP_check_validity"},
    {ERR_FUNC(OCSP_F_OCSP_MATCH_ISSUERID), "OCSP_MATCH_ISSUERID"},
    {ERR_FUNC(OCSP_F_OCSP_PARSE_URL), "OCSP_parse_url"},
    {ERR_FUNC(OCSP_F_OCSP_REQUEST_SIGN), "OCSP_request_sign"},
    {ERR_FUNC(OCSP_F_OCSP_REQUEST_VERIFY), "OCSP_request_verify"},
    {ERR_FUNC(OCSP_F_OCSP_RESPONSE_GET1_BASIC), "OCSP_response_get1_basic"},
    {ERR_FUNC(OCSP_F_OCSP_SENDREQ_BIO), "OCSP_sendreq_bio"},
    {ERR_FUNC(OCSP_F_OCSP_SENDREQ_NBIO), "OCSP_sendreq_nbio"},
    {ERR_FUNC(OCSP_F_PARSE_HTTP_LINE1), "PARSE_HTTP_LINE1"},
    {ERR_FUNC(OCSP_F_REQUEST_VERIFY), "REQUEST_VERIFY"},
    {0, NULL}
};

static ERR_STRING_DATA OCSP_str_reasons[] = {
    {ERR_REASON(OCSP_R_BAD_DATA), "bad data"},
    {ERR_REASON(OCSP_R_CERTIFICATE_VERIFY_ERROR), "certificate verify error"},
    {ERR_REASON(OCSP_R_DIGEST_ERR), "digest err"},
    {ERR_REASON(OCSP_R_ERROR_IN_NEXTUPDATE_FIELD),
     "error in nextupdate field"},
    {ERR_REASON(OCSP_R_ERROR_IN_THISUPDATE_FIELD),
     "error in thisupdate field"},
    {ERR_REASON(OCSP_R_ERROR_PARSING_URL), "error parsing url"},
    {ERR_REASON(OCSP_R_MISSING_OCSPSIGNING_USAGE),
     "missing ocspsigning usage"},
    {ERR_REASON(OCSP_R_NEXTUPDATE_BEFORE_THISUPDATE),
     "nextupdate before thisupdate"},
    {ERR_REASON(OCSP_R_NOT_BASIC_RESPONSE), "not basic response"},
    {ERR_REASON(OCSP_R_NO_CERTIFICATES_IN_CHAIN), "no certificates in chain"},
    {ERR_REASON(OCSP_R_NO_CONTENT), "no content"},
    {ERR_REASON(OCSP_R_NO_PUBLIC_KEY), "no public key"},
    {ERR_REASON(OCSP_R_NO_RESPONSE_DATA), "no response data"},
    {ERR_REASON(OCSP_R_NO_REVOKED_TIME), "no revoked time"},
    {ERR_REASON(OCSP_R_PRIVATE_KEY_DOES_NOT_MATCH_CERTIFICATE),
     "private key does not match certificate"},
    {ERR_REASON(OCSP_R_REQUEST_NOT_SIGNED), "request not signed"},
    {ERR_REASON(OCSP_R_RESPONSE_CONTAINS_NO_REVOCATION_DATA),
     "response contains no revocation data"},
    {ERR_REASON(OCSP_R_ROOT_CA_NOT_TRUSTED), "root ca not trusted"},
    {ERR_REASON(OCSP_R_SERVER_READ_ERROR), "server read error"},
    {ERR_REASON(OCSP_R_SERVER_RESPONSE_ERROR), "server response error"},
    {ERR_REASON(OCSP_R_SERVER_RESPONSE_PARSE_ERROR),
     "server response parse error"},
    {ERR_REASON(OCSP_R_SERVER_WRITE_ERROR), "server write error"},
    {ERR_REASON(OCSP_R_SIGNATURE_FAILURE), "signature failure"},
    {ERR_REASON(OCSP_R_SIGNER_CERTIFICATE_NOT_FOUND),
     "signer certificate not found"},
    {ERR_REASON(OCSP_R_STATUS_EXPIRED), "status expired"},
    {ERR_REASON(OCSP_R_STATUS_NOT_YET_VALID), "status not yet valid"},
    {ERR_REASON(OCSP_R_STATUS_TOO_OLD), "status too old"},
    {ERR_REASON(OCSP_R_UNKNOWN_MESSAGE_DIGEST), "unknown message digest"},
    {ERR_REASON(OCSP_R_UNKNOWN_NID), "unknown nid"},
    {ERR_REASON(OCSP_R_UNSUPPORTED_REQUESTORNAME_TYPE),
     "unsupported requestorname type"},
    {0, NULL}
};

#endif

void ERR_load_OCSP_strings(void)
{
#ifndef OPENSSL_NO_ERR

    if (ERR_func_error_string(OCSP_str_functs[0].error) == NULL) {
        ERR_load_strings(0, OCSP_str_functs);
        ERR_load_strings(0, OCSP_str_reasons);
    }
#endif
}
