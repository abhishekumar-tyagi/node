/*
 * Copyright 1998-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/* ====================================================================
 * Copyright 2002 Sun Microsystems, Inc. ALL RIGHTS RESERVED.
 * ECDH support in OpenSSL originally developed by
 * SUN MICROSYSTEMS, INC., and contributed to the OpenSSL project.
 */

#include "internal/cryptlib_int.h"
#include <openssl/safestack.h>

#if     defined(__i386)   || defined(__i386__)   || defined(_M_IX86) || \
        defined(__x86_64) || defined(__x86_64__) || \
        defined(_M_AMD64) || defined(_M_X64)

extern unsigned int OPENSSL_ia32cap_P[4];

# if defined(OPENSSL_CPUID_OBJ) && !defined(OPENSSL_NO_ASM) && !defined(I386_ONLY)

/*
 * Purpose of these minimalistic and character-type-agnostic subroutines
 * is to break dependency on MSVCRT (on Windows) and locale. This makes
 * OPENSSL_cpuid_setup safe to use as "constructor". "Character-type-
 * agnostic" means that they work with either wide or 8-bit characters,
 * exploiting the fact that first 127 characters can be simply casted
 * between the sets, while the rest would be simply rejected by ossl_is*
 * subroutines.
 */
#  ifdef _WIN32
typedef WCHAR variant_char;

static variant_char *ossl_getenv(const char *name)
{
    /*
     * Since we pull only one environment variable, it's simpler to
     * to just ignore |name| and use equivalent wide-char L-literal.
     * As well as to ignore excessively long values...
     */
    static WCHAR value[48];
    DWORD len = GetEnvironmentVariableW(L"OPENSSL_ia32cap", value, 48);

    return (len > 0 && len < 48) ? value : NULL;
}
#  else
typedef char variant_char;
#   define ossl_getenv getenv
#  endif

static int todigit(variant_char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;

    /* return largest base value to make caller terminate the loop */
    return 16;
}

static uint64_t ossl_strtouint64(const variant_char *str)
{
    uint64_t ret = 0;
    unsigned int digit, base = 10;

    if (*str == '0') {
        base = 8, str++;
        if (*str == 'x' || *str == 'X')
            base = 16, str++;
    }

    while((digit = todigit(*str++)) < base)
        ret = ret * base + digit;

    return ret;
}

static variant_char *ossl_strchr(const variant_char *str, char srch)
{   variant_char c;

    while((c = *str)) {
        if (c == srch)
	    return (variant_char *)str;
        str++;
    }

    return NULL;
}

#  define OPENSSL_CPUID_SETUP
typedef uint64_t IA32CAP;

void OPENSSL_cpuid_setup(void)
{
    static int trigger = 0;
    IA32CAP OPENSSL_ia32_cpuid(unsigned int *);
    IA32CAP vec;
    const variant_char *env;

    if (trigger)
        return;

    trigger = 1;
    if ((env = ossl_getenv("OPENSSL_ia32cap")) != NULL) {
        int off = (env[0] == '~') ? 1 : 0;

        vec = ossl_strtouint64(env + off);

        if (off) {
            IA32CAP mask = vec;
            vec = OPENSSL_ia32_cpuid(OPENSSL_ia32cap_P) & ~mask;
            if (mask & (1<<24)) {
                /*
                 * User disables FXSR bit, mask even other capabilities
                 * that operate exclusively on XMM, so we don't have to
                 * double-check all the time. We mask PCLMULQDQ, AMD XOP,
                 * AES-NI and AVX. Formally speaking we don't have to
                 * do it in x86_64 case, but we can safely assume that
                 * x86_64 users won't actually flip this flag.
                 */
                vec &= ~((IA32CAP)(1<<1|1<<11|1<<25|1<<28) << 32);
            }
        } else if (env[0] == ':') {
            vec = OPENSSL_ia32_cpuid(OPENSSL_ia32cap_P);
        }

        if ((env = ossl_strchr(env, ':')) != NULL) {
            IA32CAP vecx;

            env++;
            off = (env[0] == '~') ? 1 : 0;
            vecx = ossl_strtouint64(env + off);
            if (off) {
                OPENSSL_ia32cap_P[2] &= ~(unsigned int)vecx;
            } else {
                OPENSSL_ia32cap_P[2] = (unsigned int)vecx;
            }
        } else {
            OPENSSL_ia32cap_P[2] = 0;
        }
    } else {
        vec = OPENSSL_ia32_cpuid(OPENSSL_ia32cap_P);
    }

    /*
     * |(1<<10) sets a reserved bit to signal that variable
     * was initialized already... This is to avoid interference
     * with cpuid snippets in ELF .init segment.
     */
    OPENSSL_ia32cap_P[0] = (unsigned int)vec | (1 << 10);
    OPENSSL_ia32cap_P[1] = (unsigned int)(vec >> 32);
}
# else
unsigned int OPENSSL_ia32cap_P[4];
# endif
#endif
int OPENSSL_NONPIC_relocated = 0;
#if !defined(OPENSSL_CPUID_SETUP) && !defined(OPENSSL_CPUID_OBJ)
void OPENSSL_cpuid_setup(void)
{
}
#endif

#if defined(_WIN32)
# include <tchar.h>
# include <signal.h>
# ifdef __WATCOMC__
#  if defined(_UNICODE) || defined(__UNICODE__)
#   define _vsntprintf _vsnwprintf
#  else
#   define _vsntprintf _vsnprintf
#  endif
# endif
# ifdef _MSC_VER
#  define alloca _alloca
# endif

# if defined(_WIN32_WINNT) && _WIN32_WINNT>=0x0333
int OPENSSL_isservice(void)
{
    HWINSTA h;
    DWORD len;
    WCHAR *name;
    static union {
        void *p;
        FARPROC f;
    } _OPENSSL_isservice = {
        NULL
    };

    if (_OPENSSL_isservice.p == NULL) {
        HANDLE mod = GetModuleHandle(NULL);
        FARPROC f;

        if (mod != NULL)
            f = GetProcAddress(mod, "_OPENSSL_isservice");
        if (f == NULL)
            _OPENSSL_isservice.p = (void *)-1;
        else
            _OPENSSL_isservice.f = f;
    }

    if (_OPENSSL_isservice.p != (void *)-1)
        return (*_OPENSSL_isservice.f) ();

    h = GetProcessWindowStation();
    if (h == NULL)
        return -1;

    if (GetUserObjectInformationW(h, UOI_NAME, NULL, 0, &len) ||
        GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        return -1;

    if (len > 512)
        return -1;              /* paranoia */
    len++, len &= ~1;           /* paranoia */
    name = (WCHAR *)alloca(len + sizeof(WCHAR));
    if (!GetUserObjectInformationW(h, UOI_NAME, name, len, &len))
        return -1;

    len++, len &= ~1;           /* paranoia */
    name[len / sizeof(WCHAR)] = L'\0'; /* paranoia */
#  if 1
    /*
     * This doesn't cover "interactive" services [working with real
     * WinSta0's] nor programs started non-interactively by Task Scheduler
     * [those are working with SAWinSta].
     */
    if (wcsstr(name, L"Service-0x"))
        return 1;
#  else
    /* This covers all non-interactive programs such as services. */
    if (!wcsstr(name, L"WinSta0"))
        return 1;
#  endif
    else
        return 0;
}
# else
int OPENSSL_isservice(void)
{
    return 0;
}
# endif

void OPENSSL_showfatal(const char *fmta, ...)
{
    va_list ap;
    TCHAR buf[256];
    const TCHAR *fmt;
# ifdef STD_ERROR_HANDLE        /* what a dirty trick! */
    HANDLE h;

    if ((h = GetStdHandle(STD_ERROR_HANDLE)) != NULL &&
        GetFileType(h) != FILE_TYPE_UNKNOWN) {
        /* must be console application */
        int len;
        DWORD out;

        va_start(ap, fmta);
        len = _vsnprintf((char *)buf, sizeof(buf), fmta, ap);
        WriteFile(h, buf, len < 0 ? sizeof(buf) : (DWORD) len, &out, NULL);
        va_end(ap);
        return;
    }
# endif

    if (sizeof(TCHAR) == sizeof(char))
        fmt = (const TCHAR *)fmta;
    else
        do {
            int keepgoing;
            size_t len_0 = strlen(fmta) + 1, i;
            WCHAR *fmtw;

            fmtw = (WCHAR *)alloca(len_0 * sizeof(WCHAR));
            if (fmtw == NULL) {
                fmt = (const TCHAR *)L"no stack?";
                break;
            }
            if (!MultiByteToWideChar(CP_ACP, 0, fmta, len_0, fmtw, len_0))
                for (i = 0; i < len_0; i++)
                    fmtw[i] = (WCHAR)fmta[i];
            for (i = 0; i < len_0; i++) {
                if (fmtw[i] == L'%')
                    do {
                        keepgoing = 0;
                        switch (fmtw[i + 1]) {
                        case L'0':
                        case L'1':
                        case L'2':
                        case L'3':
                        case L'4':
                        case L'5':
                        case L'6':
                        case L'7':
                        case L'8':
                        case L'9':
                        case L'.':
                        case L'*':
                        case L'-':
                            i++;
                            keepgoing = 1;
                            break;
                        case L's':
                            fmtw[i + 1] = L'S';
                            break;
                        case L'S':
                            fmtw[i + 1] = L's';
                            break;
                        case L'c':
                            fmtw[i + 1] = L'C';
                            break;
                        case L'C':
                            fmtw[i + 1] = L'c';
                            break;
                        }
                    } while (keepgoing);
            }
            fmt = (const TCHAR *)fmtw;
        } while (0);

    va_start(ap, fmta);
    _vsntprintf(buf, OSSL_NELEM(buf) - 1, fmt, ap);
    buf[OSSL_NELEM(buf) - 1] = _T('\0');
    va_end(ap);

# if defined(_WIN32_WINNT) && _WIN32_WINNT>=0x0333
    /* this -------------v--- guards NT-specific calls */
    if (check_winnt() && OPENSSL_isservice() > 0) {
        HANDLE hEventLog = RegisterEventSource(NULL, _T("OpenSSL"));

        if (hEventLog != NULL) {
            const TCHAR *pmsg = buf;

            if (!ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, 0, 0, NULL,
                             1, 0, &pmsg, NULL)) {
#if defined(DEBUG)
                /*
                 * We are in a situation where we tried to report a critical
                 * error and this failed for some reason. As a last resort,
                 * in debug builds, send output to the debugger or any other
                 * tool like DebugView which can monitor the output.
                 */
                OutputDebugString(pmsg);
#endif
            }

            (void)DeregisterEventSource(hEventLog);
        }
    } else
# endif
        MessageBox(NULL, buf, _T("OpenSSL: FATAL"), MB_OK | MB_ICONERROR);
}
#else
void OPENSSL_showfatal(const char *fmta, ...)
{
#ifndef OPENSSL_NO_STDIO
    va_list ap;

    va_start(ap, fmta);
    vfprintf(stderr, fmta, ap);
    va_end(ap);
#endif
}

int OPENSSL_isservice(void)
{
    return 0;
}
#endif

void OPENSSL_die(const char *message, const char *file, int line)
{
    OPENSSL_showfatal("%s:%d: OpenSSL internal error: %s\n",
                      file, line, message);
#if !defined(_WIN32)
    abort();
#else
    /*
     * Win32 abort() customarily shows a dialog, but we just did that...
     */
# if !defined(_WIN32_WCE)
    raise(SIGABRT);
# endif
    _exit(3);
#endif
}

#if !defined(OPENSSL_CPUID_OBJ)
/* volatile unsigned char* pointers are there because
 * 1. Accessing a variable declared volatile via a pointer
 *    that lacks a volatile qualifier causes undefined behavior.
 * 2. When the variable itself is not volatile the compiler is
 *    not required to keep all those reads and can convert
 *    this into canonical memcmp() which doesn't read the whole block.
 * Pointers to volatile resolve the first problem fully. The second
 * problem cannot be resolved in any Standard-compliant way but this
 * works the problem around. Compilers typically react to
 * pointers to volatile by preserving the reads and writes through them.
 * The latter is not required by the Standard if the memory pointed to
 * is not volatile.
 * Pointers themselves are volatile in the function signature to work
 * around a subtle bug in gcc 4.6+ which causes writes through
 * pointers to volatile to not be emitted in some rare,
 * never needed in real life, pieces of code.
 */
int CRYPTO_memcmp(const volatile void * volatile in_a,
                  const volatile void * volatile in_b,
                  size_t len)
{
    size_t i;
    const volatile unsigned char *a = in_a;
    const volatile unsigned char *b = in_b;
    unsigned char x = 0;

    for (i = 0; i < len; i++)
        x |= a[i] ^ b[i];

    return x;
}
#endif
