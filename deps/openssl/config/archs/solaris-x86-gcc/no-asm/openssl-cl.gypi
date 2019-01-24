{
  'variables': {
    'openssl_defines_solaris-x86-gcc': [
      'NDEBUG',
      'FILIO_H',
      'L_ENDIAN',
      'OPENSSL_PIC',
    ],
    'openssl_cflags_solaris-x86-gcc': [
      '-Wall -O3 -fomit-frame-pointer',
      '-pthread',
      '-Wall -O3 -fomit-frame-pointer',
    ],
    'openssl_ex_libs_solaris-x86-gcc': [
      '-lsocket -lnsl -ldl -pthread',
    ],
    'openssl_cli_srcs_solaris-x86-gcc': [
      'openssl/apps/asn1pars.c',
      'openssl/apps/ca.c',
      'openssl/apps/ciphers.c',
      'openssl/apps/cms.c',
      'openssl/apps/crl.c',
      'openssl/apps/crl2p7.c',
      'openssl/apps/dgst.c',
      'openssl/apps/dhparam.c',
      'openssl/apps/dsa.c',
      'openssl/apps/dsaparam.c',
      'openssl/apps/ec.c',
      'openssl/apps/ecparam.c',
      'openssl/apps/enc.c',
      'openssl/apps/engine.c',
      'openssl/apps/errstr.c',
      'openssl/apps/gendsa.c',
      'openssl/apps/genpkey.c',
      'openssl/apps/genrsa.c',
      'openssl/apps/nseq.c',
      'openssl/apps/ocsp.c',
      'openssl/apps/openssl.c',
      'openssl/apps/passwd.c',
      'openssl/apps/pkcs12.c',
      'openssl/apps/pkcs7.c',
      'openssl/apps/pkcs8.c',
      'openssl/apps/pkey.c',
      'openssl/apps/pkeyparam.c',
      'openssl/apps/pkeyutl.c',
      'openssl/apps/prime.c',
      'openssl/apps/rand.c',
      'openssl/apps/rehash.c',
      'openssl/apps/req.c',
      'openssl/apps/rsa.c',
      'openssl/apps/rsautl.c',
      'openssl/apps/s_client.c',
      'openssl/apps/s_server.c',
      'openssl/apps/s_time.c',
      'openssl/apps/sess_id.c',
      'openssl/apps/smime.c',
      'openssl/apps/speed.c',
      'openssl/apps/spkac.c',
      'openssl/apps/srp.c',
      'openssl/apps/storeutl.c',
      'openssl/apps/ts.c',
      'openssl/apps/verify.c',
      'openssl/apps/version.c',
      'openssl/apps/x509.c',
      'openssl/apps/app_rand.c',
      'openssl/apps/apps.c',
      'openssl/apps/bf_prefix.c',
      'openssl/apps/opt.c',
      'openssl/apps/s_cb.c',
      'openssl/apps/s_socket.c',
    ],
  },
  'defines': ['<@(openssl_defines_solaris-x86-gcc)'],
  'include_dirs': [
    './include',
  ],
  'cflags' : ['<@(openssl_cflags_solaris-x86-gcc)'],
  'libraries': ['<@(openssl_ex_libs_solaris-x86-gcc)'],
  'sources': ['<@(openssl_cli_srcs_solaris-x86-gcc)'],
}
