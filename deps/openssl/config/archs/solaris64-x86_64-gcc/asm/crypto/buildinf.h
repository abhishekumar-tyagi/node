/*
 * WARNING: do not edit!
 * Generated by util/mkbuildinf.pl
 *
 * Copyright 2014-2017 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#define PLATFORM "platform: solaris64-x86_64-gcc"
#define DATE "built on: Wed Jan 31 13:06:25 2024 UTC"

/*
 * Generate compiler_flags as an array of individual characters. This is a
 * workaround for the situation where CFLAGS gets too long for a C90 string
 * literal
 */
static const char compiler_flags[] = {
    'c','o','m','p','i','l','e','r',':',' ','g','c','c',' ','-','f',
    'P','I','C',' ','-','m','6','4',' ','-','p','t','h','r','e','a',
    'd',' ','-','W','a',',','-','-','n','o','e','x','e','c','s','t',
    'a','c','k',' ','-','W','a','l','l',' ','-','O','3',' ','-','D',
    'F','I','L','I','O','_','H',' ','-','D','L','_','E','N','D','I',
    'A','N',' ','-','D','O','P','E','N','S','S','L','_','P','I','C',
    ' ','-','D','O','P','E','N','S','S','L','_','B','U','I','L','D',
    'I','N','G','_','O','P','E','N','S','S','L',' ','-','D','N','D',
    'E','B','U','G','\0'
};
