{
  'variables': {
    'openssl_sources': [
      'openssl/crypto/md5/md5_dgst.c',
      'openssl/crypto/md5/md5_one.c',
      'openssl/crypto/md5/md5_sha1.c',
      'openssl/providers/implementations/ciphers/cipher_blowfish.c',
      'openssl/providers/implementations/ciphers/cipher_blowfish_hw.c',
      'openssl/providers/implementations/ciphers/cipher_cast5.c',
      'openssl/providers/implementations/ciphers/cipher_cast5_hw.c',
      'openssl/providers/implementations/ciphers/cipher_des.c',
      'openssl/providers/implementations/ciphers/cipher_des_hw.c',
      'openssl/providers/implementations/ciphers/cipher_desx.c',
      'openssl/providers/implementations/ciphers/cipher_desx_hw.c',
      'openssl/providers/implementations/ciphers/cipher_idea.c',
      'openssl/providers/implementations/ciphers/cipher_idea_hw.c',
      'openssl/providers/implementations/ciphers/cipher_rc2.c',
      'openssl/providers/implementations/ciphers/cipher_rc2_hw.c',
      'openssl/providers/implementations/ciphers/cipher_rc4.c',
      'openssl/providers/implementations/ciphers/cipher_rc4_hmac_md5.c',
      'openssl/providers/implementations/ciphers/cipher_rc4_hmac_md5_hw.c',
      'openssl/providers/implementations/ciphers/cipher_rc4_hw.c',
      'openssl/providers/implementations/ciphers/cipher_seed.c',
      'openssl/providers/implementations/ciphers/cipher_seed_hw.c',
      'openssl/providers/implementations/ciphers/cipher_tdes_common.c',
      'openssl/providers/implementations/digests/md4_prov.c',
      'openssl/providers/implementations/digests/mdc2_prov.c',
      'openssl/providers/implementations/digests/ripemd_prov.c',
      'openssl/providers/implementations/digests/wp_prov.c',
      'openssl/providers/implementations/kdfs/pbkdf1.c',
      'openssl/providers/prov_running.c',
      'openssl/providers/legacyprov.c',

    ],
    'openssl_sources_linux-elf': [

    ],
    'openssl_defines_linux-elf': [
      'NDEBUG',
      'OPENSSL_USE_NODELETE',
      'L_ENDIAN',
      'OPENSSL_BUILDING_OPENSSL',
      'DES_ASM',
      'MD5_ASM',
      'OPENSSL_BN_ASM_GF2m',
      'OPENSSL_BN_ASM_MONT',
      'OPENSSL_BN_ASM_PART_WORDS',
      'OPENSSL_IA32_SSE2',
    ],
    'openssl_cflags_linux-elf': [
      '-Wa,--noexecstack',
      '-Wall -O3 -fomit-frame-pointer',
      '-pthread',
      '-Wall -O3 -fomit-frame-pointer',
    ],
    'openssl_ex_libs_linux-elf': [
      '-ldl -pthread',
    ],
    'version_script': '$(srcdir)/deps/openssl/config/archs/linux-elf/asm/providers/legacy.ld'
  },
  'include_dirs': [
    '.',
    './include',
    './crypto',
    './crypto/include/internal',
    './providers/common/include',
  ],
  'defines': ['<@(openssl_defines_linux-elf)'],
  'cflags': ['<@(openssl_cflags_linux-elf)'],
  'libraries': ['<@(openssl_ex_libs_linux-elf)'],
  'ldflags': ['-Wl,--version-script=<@(version_script)'],
  'sources': ['<@(openssl_sources)', '<@(openssl_sources_linux-elf)'],
  'direct_dependent_settings': {
    'include_dirs': ['./include', '.'],
    'defines': ['<@(openssl_defines_linux-elf)'],
  },
}
