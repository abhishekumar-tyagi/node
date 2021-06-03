/*
 * Copyright 2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "prov/der_ecx.h"

/* Well known OIDs precompiled */

/*
 * id-X25519        OBJECT IDENTIFIER ::= { id-edwards-curve-algs 110 }
 */
const unsigned char ossl_der_oid_id_X25519[DER_OID_SZ_id_X25519] = {
    DER_OID_V_id_X25519
};

/*
 * id-X448          OBJECT IDENTIFIER ::= { id-edwards-curve-algs 111 }
 */
const unsigned char ossl_der_oid_id_X448[DER_OID_SZ_id_X448] = {
    DER_OID_V_id_X448
};

/*
 * id-Ed25519       OBJECT IDENTIFIER ::= { id-edwards-curve-algs 112 }
 */
const unsigned char ossl_der_oid_id_Ed25519[DER_OID_SZ_id_Ed25519] = {
    DER_OID_V_id_Ed25519
};

/*
 * id-Ed448         OBJECT IDENTIFIER ::= { id-edwards-curve-algs 113 }
 */
const unsigned char ossl_der_oid_id_Ed448[DER_OID_SZ_id_Ed448] = {
    DER_OID_V_id_Ed448
};

