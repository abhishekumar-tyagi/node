/*
 * WARNING: do not edit!
 * Generated by apps/progs.pl
 *
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

typedef enum FUNC_TYPE {
    FT_none, FT_general, FT_md, FT_cipher, FT_pkey,
    FT_md_alg, FT_cipher_alg
} FUNC_TYPE;

typedef struct function_st {
    FUNC_TYPE type;
    const char *name;
    int (*func)(int argc, char *argv[]);
    const OPTIONS *help;
} FUNCTION;

DEFINE_LHASH_OF(FUNCTION);

extern int asn1parse_main(int argc, char *argv[]);
extern int ca_main(int argc, char *argv[]);
extern int ciphers_main(int argc, char *argv[]);
extern int cms_main(int argc, char *argv[]);
extern int crl_main(int argc, char *argv[]);
extern int crl2pkcs7_main(int argc, char *argv[]);
extern int dgst_main(int argc, char *argv[]);
extern int dhparam_main(int argc, char *argv[]);
extern int dsa_main(int argc, char *argv[]);
extern int dsaparam_main(int argc, char *argv[]);
extern int ec_main(int argc, char *argv[]);
extern int ecparam_main(int argc, char *argv[]);
extern int enc_main(int argc, char *argv[]);
extern int engine_main(int argc, char *argv[]);
extern int errstr_main(int argc, char *argv[]);
extern int exit_main(int argc, char *argv[]);
extern int gendsa_main(int argc, char *argv[]);
extern int genpkey_main(int argc, char *argv[]);
extern int genrsa_main(int argc, char *argv[]);
extern int help_main(int argc, char *argv[]);
extern int list_main(int argc, char *argv[]);
extern int nseq_main(int argc, char *argv[]);
extern int ocsp_main(int argc, char *argv[]);
extern int passwd_main(int argc, char *argv[]);
extern int pkcs12_main(int argc, char *argv[]);
extern int pkcs7_main(int argc, char *argv[]);
extern int pkcs8_main(int argc, char *argv[]);
extern int pkey_main(int argc, char *argv[]);
extern int pkeyparam_main(int argc, char *argv[]);
extern int pkeyutl_main(int argc, char *argv[]);
extern int prime_main(int argc, char *argv[]);
extern int rand_main(int argc, char *argv[]);
extern int rehash_main(int argc, char *argv[]);
extern int req_main(int argc, char *argv[]);
extern int rsa_main(int argc, char *argv[]);
extern int rsautl_main(int argc, char *argv[]);
extern int s_client_main(int argc, char *argv[]);
extern int s_server_main(int argc, char *argv[]);
extern int s_time_main(int argc, char *argv[]);
extern int sess_id_main(int argc, char *argv[]);
extern int smime_main(int argc, char *argv[]);
extern int speed_main(int argc, char *argv[]);
extern int spkac_main(int argc, char *argv[]);
extern int srp_main(int argc, char *argv[]);
extern int ts_main(int argc, char *argv[]);
extern int verify_main(int argc, char *argv[]);
extern int version_main(int argc, char *argv[]);
extern int x509_main(int argc, char *argv[]);

extern OPTIONS asn1parse_options[];
extern OPTIONS ca_options[];
extern OPTIONS ciphers_options[];
extern OPTIONS cms_options[];
extern OPTIONS crl_options[];
extern OPTIONS crl2pkcs7_options[];
extern OPTIONS dgst_options[];
extern OPTIONS dhparam_options[];
extern OPTIONS dsa_options[];
extern OPTIONS dsaparam_options[];
extern OPTIONS ec_options[];
extern OPTIONS ecparam_options[];
extern OPTIONS enc_options[];
extern OPTIONS engine_options[];
extern OPTIONS errstr_options[];
extern OPTIONS exit_options[];
extern OPTIONS gendsa_options[];
extern OPTIONS genpkey_options[];
extern OPTIONS genrsa_options[];
extern OPTIONS help_options[];
extern OPTIONS list_options[];
extern OPTIONS nseq_options[];
extern OPTIONS ocsp_options[];
extern OPTIONS passwd_options[];
extern OPTIONS pkcs12_options[];
extern OPTIONS pkcs7_options[];
extern OPTIONS pkcs8_options[];
extern OPTIONS pkey_options[];
extern OPTIONS pkeyparam_options[];
extern OPTIONS pkeyutl_options[];
extern OPTIONS prime_options[];
extern OPTIONS rand_options[];
extern OPTIONS rehash_options[];
extern OPTIONS req_options[];
extern OPTIONS rsa_options[];
extern OPTIONS rsautl_options[];
extern OPTIONS s_client_options[];
extern OPTIONS s_server_options[];
extern OPTIONS s_time_options[];
extern OPTIONS sess_id_options[];
extern OPTIONS smime_options[];
extern OPTIONS speed_options[];
extern OPTIONS spkac_options[];
extern OPTIONS srp_options[];
extern OPTIONS ts_options[];
extern OPTIONS verify_options[];
extern OPTIONS version_options[];
extern OPTIONS x509_options[];

#ifdef INCLUDE_FUNCTION_TABLE
static FUNCTION functions[] = {
    { FT_general, "asn1parse", asn1parse_main, asn1parse_options },
    { FT_general, "ca", ca_main, ca_options },
#ifndef OPENSSL_NO_SOCK
    { FT_general, "ciphers", ciphers_main, ciphers_options },
#endif
#ifndef OPENSSL_NO_CMS
    { FT_general, "cms", cms_main, cms_options },
#endif
    { FT_general, "crl", crl_main, crl_options },
    { FT_general, "crl2pkcs7", crl2pkcs7_main, crl2pkcs7_options },
    { FT_general, "dgst", dgst_main, dgst_options },
#ifndef OPENSSL_NO_DH
    { FT_general, "dhparam", dhparam_main, dhparam_options },
#endif
#ifndef OPENSSL_NO_DSA
    { FT_general, "dsa", dsa_main, dsa_options },
#endif
#ifndef OPENSSL_NO_DSA
    { FT_general, "dsaparam", dsaparam_main, dsaparam_options },
#endif
#ifndef OPENSSL_NO_EC
    { FT_general, "ec", ec_main, ec_options },
#endif
#ifndef OPENSSL_NO_EC
    { FT_general, "ecparam", ecparam_main, ecparam_options },
#endif
    { FT_general, "enc", enc_main, enc_options },
#ifndef OPENSSL_NO_ENGINE
    { FT_general, "engine", engine_main, engine_options },
#endif
    { FT_general, "errstr", errstr_main, errstr_options },
    { FT_general, "exit", exit_main, exit_options },
#ifndef OPENSSL_NO_DSA
    { FT_general, "gendsa", gendsa_main, gendsa_options },
#endif
    { FT_general, "genpkey", genpkey_main, genpkey_options },
#ifndef OPENSSL_NO_RSA
    { FT_general, "genrsa", genrsa_main, genrsa_options },
#endif
    { FT_general, "help", help_main, help_options },
    { FT_general, "list", list_main, list_options },
    { FT_general, "nseq", nseq_main, nseq_options },
#ifndef OPENSSL_NO_OCSP
    { FT_general, "ocsp", ocsp_main, ocsp_options },
#endif
    { FT_general, "passwd", passwd_main, passwd_options },
#ifndef OPENSSL_NO_DES
    { FT_general, "pkcs12", pkcs12_main, pkcs12_options },
#endif
    { FT_general, "pkcs7", pkcs7_main, pkcs7_options },
    { FT_general, "pkcs8", pkcs8_main, pkcs8_options },
    { FT_general, "pkey", pkey_main, pkey_options },
    { FT_general, "pkeyparam", pkeyparam_main, pkeyparam_options },
    { FT_general, "pkeyutl", pkeyutl_main, pkeyutl_options },
    { FT_general, "prime", prime_main, prime_options },
    { FT_general, "rand", rand_main, rand_options },
    { FT_general, "rehash", rehash_main, rehash_options },
    { FT_general, "req", req_main, req_options },
    { FT_general, "rsa", rsa_main, rsa_options },
#ifndef OPENSSL_NO_RSA
    { FT_general, "rsautl", rsautl_main, rsautl_options },
#endif
#ifndef OPENSSL_NO_SOCK
    { FT_general, "s_client", s_client_main, s_client_options },
#endif
#ifndef OPENSSL_NO_SOCK
    { FT_general, "s_server", s_server_main, s_server_options },
#endif
#ifndef OPENSSL_NO_SOCK
    { FT_general, "s_time", s_time_main, s_time_options },
#endif
    { FT_general, "sess_id", sess_id_main, sess_id_options },
    { FT_general, "smime", smime_main, smime_options },
    { FT_general, "speed", speed_main, speed_options },
    { FT_general, "spkac", spkac_main, spkac_options },
#ifndef OPENSSL_NO_SRP
    { FT_general, "srp", srp_main, srp_options },
#endif
#ifndef OPENSSL_NO_TS
    { FT_general, "ts", ts_main, ts_options },
#endif
    { FT_general, "verify", verify_main, verify_options },
    { FT_general, "version", version_main, version_options },
    { FT_general, "x509", x509_main, x509_options },
#ifndef OPENSSL_NO_MD2
    { FT_md, "md2", dgst_main},
#endif
#ifndef OPENSSL_NO_MD4
    { FT_md, "md4", dgst_main},
#endif
    { FT_md, "md5", dgst_main},
#ifndef OPENSSL_NO_GOST
    { FT_md, "gost", dgst_main},
#endif
    { FT_md, "sha1", dgst_main},
    { FT_md, "sha224", dgst_main},
    { FT_md, "sha256", dgst_main},
    { FT_md, "sha384", dgst_main},
    { FT_md, "sha512", dgst_main},
#ifndef OPENSSL_NO_MDC2
    { FT_md, "mdc2", dgst_main},
#endif
#ifndef OPENSSL_NO_RMD160
    { FT_md, "rmd160", dgst_main},
#endif
#ifndef OPENSSL_NO_BLAKE2
    { FT_md, "blake2b512", dgst_main},
#endif
#ifndef OPENSSL_NO_BLAKE2
    { FT_md, "blake2s256", dgst_main},
#endif
    { FT_cipher, "aes-128-cbc", enc_main, enc_options },
    { FT_cipher, "aes-128-ecb", enc_main, enc_options },
    { FT_cipher, "aes-192-cbc", enc_main, enc_options },
    { FT_cipher, "aes-192-ecb", enc_main, enc_options },
    { FT_cipher, "aes-256-cbc", enc_main, enc_options },
    { FT_cipher, "aes-256-ecb", enc_main, enc_options },
#ifndef OPENSSL_NO_CAMELLIA
    { FT_cipher, "camellia-128-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_CAMELLIA
    { FT_cipher, "camellia-128-ecb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_CAMELLIA
    { FT_cipher, "camellia-192-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_CAMELLIA
    { FT_cipher, "camellia-192-ecb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_CAMELLIA
    { FT_cipher, "camellia-256-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_CAMELLIA
    { FT_cipher, "camellia-256-ecb", enc_main, enc_options },
#endif
    { FT_cipher, "base64", enc_main, enc_options },
#ifdef ZLIB
    { FT_cipher, "zlib", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des3", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "desx", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_IDEA
    { FT_cipher, "idea", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_SEED
    { FT_cipher, "seed", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC4
    { FT_cipher, "rc4", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC4
    { FT_cipher, "rc4-40", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC2
    { FT_cipher, "rc2", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_BF
    { FT_cipher, "bf", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_CAST
    { FT_cipher, "cast", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC5
    { FT_cipher, "rc5", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-ecb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-ede", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-ede3", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-ede-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-ede3-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-cfb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-ede-cfb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-ede3-cfb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-ofb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-ede-ofb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_DES
    { FT_cipher, "des-ede3-ofb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_IDEA
    { FT_cipher, "idea-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_IDEA
    { FT_cipher, "idea-ecb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_IDEA
    { FT_cipher, "idea-cfb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_IDEA
    { FT_cipher, "idea-ofb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_SEED
    { FT_cipher, "seed-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_SEED
    { FT_cipher, "seed-ecb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_SEED
    { FT_cipher, "seed-cfb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_SEED
    { FT_cipher, "seed-ofb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC2
    { FT_cipher, "rc2-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC2
    { FT_cipher, "rc2-ecb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC2
    { FT_cipher, "rc2-cfb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC2
    { FT_cipher, "rc2-ofb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC2
    { FT_cipher, "rc2-64-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC2
    { FT_cipher, "rc2-40-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_BF
    { FT_cipher, "bf-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_BF
    { FT_cipher, "bf-ecb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_BF
    { FT_cipher, "bf-cfb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_BF
    { FT_cipher, "bf-ofb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_CAST
    { FT_cipher, "cast5-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_CAST
    { FT_cipher, "cast5-ecb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_CAST
    { FT_cipher, "cast5-cfb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_CAST
    { FT_cipher, "cast5-ofb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_CAST
    { FT_cipher, "cast-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC5
    { FT_cipher, "rc5-cbc", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC5
    { FT_cipher, "rc5-ecb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC5
    { FT_cipher, "rc5-cfb", enc_main, enc_options },
#endif
#ifndef OPENSSL_NO_RC5
    { FT_cipher, "rc5-ofb", enc_main, enc_options },
#endif
    { 0, NULL, NULL}
};
#endif
