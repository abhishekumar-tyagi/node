/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/* Expose glibc-specific EAI_* error codes. Needs to be defined before we
 * include any headers.
 */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include "uv.h"
#include "internal.h"

#include <errno.h>
#include <stddef.h> /* NULL */
#include <stdlib.h>
#include <string.h>
#include <net/if.h> /* if_indextoname() */

/* EAI_* constants. */
#include <netdb.h>


int uv__getaddrinfo_translate_error(int sys_err) {
  switch (sys_err) {
  case 0: return 0;
#if defined(EAI_ADDRFAMILY)
  case EAI_ADDRFAMILY: return UV_EAI_ADDRFAMILY;
#endif
#if defined(EAI_AGAIN)
  case EAI_AGAIN: return UV_EAI_AGAIN;
#endif
#if defined(EAI_BADFLAGS)
  case EAI_BADFLAGS: return UV_EAI_BADFLAGS;
#endif
#if defined(EAI_BADHINTS)
  case EAI_BADHINTS: return UV_EAI_BADHINTS;
#endif
#if defined(EAI_CANCELED)
  case EAI_CANCELED: return UV_EAI_CANCELED;
#endif
#if defined(EAI_FAIL)
  case EAI_FAIL: return UV_EAI_FAIL;
#endif
#if defined(EAI_FAMILY)
  case EAI_FAMILY: return UV_EAI_FAMILY;
#endif
#if defined(EAI_MEMORY)
  case EAI_MEMORY: return UV_EAI_MEMORY;
#endif
#if defined(EAI_NODATA)
  case EAI_NODATA: return UV_EAI_NODATA;
#endif
#if defined(EAI_NONAME)
# if !defined(EAI_NODATA) || EAI_NODATA != EAI_NONAME
  case EAI_NONAME: return UV_EAI_NONAME;
# endif
#endif
#if defined(EAI_OVERFLOW)
  case EAI_OVERFLOW: return UV_EAI_OVERFLOW;
#endif
#if defined(EAI_PROTOCOL)
  case EAI_PROTOCOL: return UV_EAI_PROTOCOL;
#endif
#if defined(EAI_SERVICE)
  case EAI_SERVICE: return UV_EAI_SERVICE;
#endif
#if defined(EAI_SOCKTYPE)
  case EAI_SOCKTYPE: return UV_EAI_SOCKTYPE;
#endif
#if defined(EAI_SYSTEM)
  case EAI_SYSTEM: return UV__ERR(errno);
#endif
  }
  assert(!"unknown EAI_* error code");
  abort();
  return 0;  /* Pacify compiler. */
}


static void uv__getaddrinfo_work(struct uv__work* w) {
  uv_getaddrinfo_t* req;
  int err;

  req = container_of(w, uv_getaddrinfo_t, work_req);
  err = getaddrinfo(req->hostname, req->service, req->hints, &req->addrinfo);
  req->retcode = uv__getaddrinfo_translate_error(err);
}


static void uv__getaddrinfo_done(struct uv__work* w, int status) {
  uv_getaddrinfo_t* req;

  req = container_of(w, uv_getaddrinfo_t, work_req);
  uv__req_unregister(req->loop, req);

  /* See initialization in uv_getaddrinfo(). */
  if (req->hints)
    uv__free(req->hints);
  else if (req->service)
    uv__free(req->service);
  else if (req->hostname)
    uv__free(req->hostname);
  else
    assert(0);

  req->hints = NULL;
  req->service = NULL;
  req->hostname = NULL;

  if (status == UV_ECANCELED) {
    assert(req->retcode == 0);
    req->retcode = UV_EAI_CANCELED;
  }

  if (req->cb != NULL)
    req->cb(req, req->retcode, req->addrinfo);
}

static void uv__getaddrinfo_executor_work(uv_work_t* req) {
  uv__getaddrinfo_work(&((uv_getaddrinfo_t*) req->data)->work_req);
}

static void uv__getaddrinfo_executor_done(uv_work_t* req, int status) {
  uv__getaddrinfo_done(&((uv_getaddrinfo_t*) req->data)->work_req, status);
  uv__free(req);
}

int uv_getaddrinfo(uv_loop_t* loop,
                   uv_getaddrinfo_t* req,
                   uv_getaddrinfo_cb getaddrinfo_cb,
                   const char* hostname,
                   const char* service,
                   const struct addrinfo* hints) {
  uv_work_t* work;
  uv_work_options_t options;
  size_t hostname_len;
  size_t service_len;
  size_t hints_len;
  size_t len;
  char* buf;

  if (req == NULL || (hostname == NULL && service == NULL))
    return UV_EINVAL;

  hostname_len = hostname ? strlen(hostname) + 1 : 0;
  service_len = service ? strlen(service) + 1 : 0;
  hints_len = hints ? sizeof(*hints) : 0;

  buf = uv__malloc(hostname_len + service_len + hints_len);
  if (buf == NULL)
    return UV_ENOMEM;

  work = NULL;
  if (getaddrinfo_cb != NULL) {
    work = uv__malloc(sizeof(*work));
    if (work == NULL) {
      uv__free(buf);
      return UV_ENOMEM;
    }
  }

  uv__req_init(loop, req, UV_GETADDRINFO);
  req->loop = loop;
  req->cb = getaddrinfo_cb;
  req->addrinfo = NULL;
  req->hints = NULL;
  req->service = NULL;
  req->hostname = NULL;
  req->retcode = 0;

  /* order matters, see uv_getaddrinfo_done() */
  len = 0;

  if (hints) {
    req->hints = memcpy(buf + len, hints, sizeof(*hints));
    len += sizeof(*hints);
  }

  if (service) {
    req->service = memcpy(buf + len, service, service_len);
    len += service_len;
  }

  if (hostname)
    req->hostname = memcpy(buf + len, hostname, hostname_len);

  if (getaddrinfo_cb != NULL) {
    work->data = req;
    req->executor_data = work; /* For uv_cancel. */
    options.type = UV_WORK_DNS;
    options.priority = -1;
    options.cancelable = 0;
    options.data = NULL;
    LOG_2("getaddrinfo: req %p work %p\n", req, work);
    uv_executor_queue_work(loop,
                           work,
                           &options,
                           uv__getaddrinfo_executor_work,
                           uv__getaddrinfo_executor_done);
    return 0;
  } else {
    /* TODO uv__getaddrinfo_work and done APIs should take req directly. Windows too. */
    uv__getaddrinfo_work(&req->work_req);
    uv__getaddrinfo_done(&req->work_req, 0);
    return req->retcode;
  }
}


void uv_freeaddrinfo(struct addrinfo* ai) {
  if (ai)
    freeaddrinfo(ai);
}


int uv_if_indextoname(unsigned int ifindex, char* buffer, size_t* size) {
  char ifname_buf[UV_IF_NAMESIZE];
  size_t len;

  if (buffer == NULL || size == NULL || *size == 0)
    return UV_EINVAL;

  if (if_indextoname(ifindex, ifname_buf) == NULL)
    return UV__ERR(errno);

  len = strnlen(ifname_buf, sizeof(ifname_buf));

  if (*size <= len) {
    *size = len + 1;
    return UV_ENOBUFS;
  }

  memcpy(buffer, ifname_buf, len);
  buffer[len] = '\0';
  *size = len;

  return 0;
}

int uv_if_indextoiid(unsigned int ifindex, char* buffer, size_t* size) {
  return uv_if_indextoname(ifindex, buffer, size);
}
