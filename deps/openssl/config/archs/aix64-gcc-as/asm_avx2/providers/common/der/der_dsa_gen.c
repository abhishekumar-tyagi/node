/*
 * Copyright 2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/*
 * DSA low level APIs are deprecated for public use, but still ok for
 * internal use.
 */
#include "internal/deprecated.h"

#include "prov/der_dsa.h"

/* Well known OIDs precompiled */

/*
 * id-dsa OBJECT IDENTIFIER ::= {
 *      iso(1) member-body(2) us(840) x9-57(10040) x9algorithm(4) 1 }
 */
const unsigned char ossl_der_oid_id_dsa[DER_OID_SZ_id_dsa] = {
    DER_OID_V_id_dsa
};

/*
 * id-dsa-with-sha1 OBJECT IDENTIFIER ::=  {
 *      iso(1) member-body(2) us(840) x9-57 (10040) x9algorithm(4) 3 }
 */
const unsigned char ossl_der_oid_id_dsa_with_sha1[DER_OID_SZ_id_dsa_with_sha1] = {
    DER_OID_V_id_dsa_with_sha1
};

/*
 * id-dsa-with-sha224 OBJECT IDENTIFIER ::= { sigAlgs 1 }
 */
const unsigned char ossl_der_oid_id_dsa_with_sha224[DER_OID_SZ_id_dsa_with_sha224] = {
    DER_OID_V_id_dsa_with_sha224
};

/*
 * id-dsa-with-sha256 OBJECT IDENTIFIER ::= { sigAlgs 2 }
 */
const unsigned char ossl_der_oid_id_dsa_with_sha256[DER_OID_SZ_id_dsa_with_sha256] = {
    DER_OID_V_id_dsa_with_sha256
};

/*
 * id-dsa-with-sha384 OBJECT IDENTIFIER ::= { sigAlgs 3 }
 */
const unsigned char ossl_der_oid_id_dsa_with_sha384[DER_OID_SZ_id_dsa_with_sha384] = {
    DER_OID_V_id_dsa_with_sha384
};

/*
 * id-dsa-with-sha512 OBJECT IDENTIFIER ::= { sigAlgs 4 }
 */
const unsigned char ossl_der_oid_id_dsa_with_sha512[DER_OID_SZ_id_dsa_with_sha512] = {
    DER_OID_V_id_dsa_with_sha512
};

/*
 * id-dsa-with-sha3-224 OBJECT IDENTIFIER ::= { sigAlgs 5 }
 */
const unsigned char ossl_der_oid_id_dsa_with_sha3_224[DER_OID_SZ_id_dsa_with_sha3_224] = {
    DER_OID_V_id_dsa_with_sha3_224
};

/*
 * id-dsa-with-sha3-256 OBJECT IDENTIFIER ::= { sigAlgs 6 }
 */
const unsigned char ossl_der_oid_id_dsa_with_sha3_256[DER_OID_SZ_id_dsa_with_sha3_256] = {
    DER_OID_V_id_dsa_with_sha3_256
};

/*
 * id-dsa-with-sha3-384 OBJECT IDENTIFIER ::= { sigAlgs 7 }
 */
const unsigned char ossl_der_oid_id_dsa_with_sha3_384[DER_OID_SZ_id_dsa_with_sha3_384] = {
    DER_OID_V_id_dsa_with_sha3_384
};

/*
 * id-dsa-with-sha3-512 OBJECT IDENTIFIER ::= { sigAlgs 8 }
 */
const unsigned char ossl_der_oid_id_dsa_with_sha3_512[DER_OID_SZ_id_dsa_with_sha3_512] = {
    DER_OID_V_id_dsa_with_sha3_512
};

