/*
 * WARNING: do not edit!
 * Generated by util/mkbuildinf.pl
 *
 * Copyright 2014-2017 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#define PLATFORM "platform: BSD-x86_64"
#define DATE "built on: Thu Nov 22 19:33:04 2018 UTC"

/*
 * Generate compiler_flags as an array of individual characters. This is a
 * workaround for the situation where CFLAGS gets too long for a C90 string
 * literal
 */
static const char compiler_flags[] = {
    'c','o','m','p','i','l','e','r',':',' ','c','c',' ','-','f','P',
    'I','C',' ','-','p','t','h','r','e','a','d',' ','-','W','a','l',
    'l',' ','-','O','3',' ','-','D','L','_','E','N','D','I','A','N',
    ' ','-','D','O','P','E','N','S','S','L','_','P','I','C',' ','-',
    'D','_','T','H','R','E','A','D','_','S','A','F','E',' ','-','D',
    '_','R','E','E','N','T','R','A','N','T',' ','-','D','N','D','E',
    'B','U','G','\0'
};
