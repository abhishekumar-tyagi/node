/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2013 Tatsuhiro Tsujikawa
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef NGHTTP2_HD_H
#define NGHTTP2_HD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <nghttp2/nghttp2.h>

#include "nghttp2_hd_huffman.h"
#include "nghttp2_buf.h"
#include "nghttp2_mem.h"
#include "nghttp2_rcbuf.h"

#define NGHTTP2_HD_DEFAULT_MAX_BUFFER_SIZE NGHTTP2_DEFAULT_HEADER_TABLE_SIZE
#define NGHTTP2_HD_ENTRY_OVERHEAD 32

/* The maximum length of one name/value pair.  This is the sum of the
   length of name and value.  This is not specified by the spec. We
   just chose the arbitrary size */
#define NGHTTP2_HD_MAX_NV 65536

/* Default size of maximum table buffer size for encoder. Even if
   remote decoder notifies larger buffer size for its decoding,
   encoder only uses the memory up to this value. */
#define NGHTTP2_HD_DEFAULT_MAX_DEFLATE_BUFFER_SIZE (1 << 12)

/* Exported for unit test */
#define NGHTTP2_STATIC_TABLE_LENGTH 61

/* Generated by genlibtokenlookup.py */
typedef enum {
  NGHTTP2_TOKEN__AUTHORITY = 0,
  NGHTTP2_TOKEN__METHOD = 1,
  NGHTTP2_TOKEN__PATH = 3,
  NGHTTP2_TOKEN__SCHEME = 5,
  NGHTTP2_TOKEN__STATUS = 7,
  NGHTTP2_TOKEN_ACCEPT_CHARSET = 14,
  NGHTTP2_TOKEN_ACCEPT_ENCODING = 15,
  NGHTTP2_TOKEN_ACCEPT_LANGUAGE = 16,
  NGHTTP2_TOKEN_ACCEPT_RANGES = 17,
  NGHTTP2_TOKEN_ACCEPT = 18,
  NGHTTP2_TOKEN_ACCESS_CONTROL_ALLOW_ORIGIN = 19,
  NGHTTP2_TOKEN_AGE = 20,
  NGHTTP2_TOKEN_ALLOW = 21,
  NGHTTP2_TOKEN_AUTHORIZATION = 22,
  NGHTTP2_TOKEN_CACHE_CONTROL = 23,
  NGHTTP2_TOKEN_CONTENT_DISPOSITION = 24,
  NGHTTP2_TOKEN_CONTENT_ENCODING = 25,
  NGHTTP2_TOKEN_CONTENT_LANGUAGE = 26,
  NGHTTP2_TOKEN_CONTENT_LENGTH = 27,
  NGHTTP2_TOKEN_CONTENT_LOCATION = 28,
  NGHTTP2_TOKEN_CONTENT_RANGE = 29,
  NGHTTP2_TOKEN_CONTENT_TYPE = 30,
  NGHTTP2_TOKEN_COOKIE = 31,
  NGHTTP2_TOKEN_DATE = 32,
  NGHTTP2_TOKEN_ETAG = 33,
  NGHTTP2_TOKEN_EXPECT = 34,
  NGHTTP2_TOKEN_EXPIRES = 35,
  NGHTTP2_TOKEN_FROM = 36,
  NGHTTP2_TOKEN_HOST = 37,
  NGHTTP2_TOKEN_IF_MATCH = 38,
  NGHTTP2_TOKEN_IF_MODIFIED_SINCE = 39,
  NGHTTP2_TOKEN_IF_NONE_MATCH = 40,
  NGHTTP2_TOKEN_IF_RANGE = 41,
  NGHTTP2_TOKEN_IF_UNMODIFIED_SINCE = 42,
  NGHTTP2_TOKEN_LAST_MODIFIED = 43,
  NGHTTP2_TOKEN_LINK = 44,
  NGHTTP2_TOKEN_LOCATION = 45,
  NGHTTP2_TOKEN_MAX_FORWARDS = 46,
  NGHTTP2_TOKEN_PROXY_AUTHENTICATE = 47,
  NGHTTP2_TOKEN_PROXY_AUTHORIZATION = 48,
  NGHTTP2_TOKEN_RANGE = 49,
  NGHTTP2_TOKEN_REFERER = 50,
  NGHTTP2_TOKEN_REFRESH = 51,
  NGHTTP2_TOKEN_RETRY_AFTER = 52,
  NGHTTP2_TOKEN_SERVER = 53,
  NGHTTP2_TOKEN_SET_COOKIE = 54,
  NGHTTP2_TOKEN_STRICT_TRANSPORT_SECURITY = 55,
  NGHTTP2_TOKEN_TRANSFER_ENCODING = 56,
  NGHTTP2_TOKEN_USER_AGENT = 57,
  NGHTTP2_TOKEN_VARY = 58,
  NGHTTP2_TOKEN_VIA = 59,
  NGHTTP2_TOKEN_WWW_AUTHENTICATE = 60,
  NGHTTP2_TOKEN_TE,
  NGHTTP2_TOKEN_CONNECTION,
  NGHTTP2_TOKEN_KEEP_ALIVE,
  NGHTTP2_TOKEN_PROXY_CONNECTION,
  NGHTTP2_TOKEN_UPGRADE,
} nghttp2_token;

struct nghttp2_hd_entry;
typedef struct nghttp2_hd_entry nghttp2_hd_entry;

typedef struct {
  /* The buffer containing header field name.  NULL-termination is
     guaranteed. */
  nghttp2_rcbuf *name;
  /* The buffer containing header field value.  NULL-termination is
     guaranteed. */
  nghttp2_rcbuf *value;
  /* nghttp2_token value for name.  It could be -1 if we have no token
     for that header field name. */
  int32_t token;
  /* Bitwise OR of one or more of nghttp2_nv_flag. */
  uint8_t flags;
} nghttp2_hd_nv;

struct nghttp2_hd_entry {
  /* The header field name/value pair */
  nghttp2_hd_nv nv;
  /* This is solely for nghttp2_hd_{deflate,inflate}_get_table_entry
     APIs to keep backward compatibility. */
  nghttp2_nv cnv;
  /* The next entry which shares same bucket in hash table. */
  nghttp2_hd_entry *next;
  /* The sequence number.  We will increment it by one whenever we
     store nghttp2_hd_entry to dynamic header table. */
  uint32_t seq;
  /* The hash value for header name (nv.name). */
  uint32_t hash;
};

/* The entry used for static header table. */
typedef struct {
  nghttp2_rcbuf name;
  nghttp2_rcbuf value;
  nghttp2_nv cnv;
  int32_t token;
  uint32_t hash;
} nghttp2_hd_static_entry;

typedef struct {
  nghttp2_hd_entry **buffer;
  size_t mask;
  size_t first;
  size_t len;
} nghttp2_hd_ringbuf;

typedef enum {
  NGHTTP2_HD_OPCODE_NONE,
  NGHTTP2_HD_OPCODE_INDEXED,
  NGHTTP2_HD_OPCODE_NEWNAME,
  NGHTTP2_HD_OPCODE_INDNAME
} nghttp2_hd_opcode;

typedef enum {
  NGHTTP2_HD_STATE_EXPECT_TABLE_SIZE,
  NGHTTP2_HD_STATE_INFLATE_START,
  NGHTTP2_HD_STATE_OPCODE,
  NGHTTP2_HD_STATE_READ_TABLE_SIZE,
  NGHTTP2_HD_STATE_READ_INDEX,
  NGHTTP2_HD_STATE_NEWNAME_CHECK_NAMELEN,
  NGHTTP2_HD_STATE_NEWNAME_READ_NAMELEN,
  NGHTTP2_HD_STATE_NEWNAME_READ_NAMEHUFF,
  NGHTTP2_HD_STATE_NEWNAME_READ_NAME,
  NGHTTP2_HD_STATE_CHECK_VALUELEN,
  NGHTTP2_HD_STATE_READ_VALUELEN,
  NGHTTP2_HD_STATE_READ_VALUEHUFF,
  NGHTTP2_HD_STATE_READ_VALUE
} nghttp2_hd_inflate_state;

typedef enum {
  NGHTTP2_HD_WITH_INDEXING,
  NGHTTP2_HD_WITHOUT_INDEXING,
  NGHTTP2_HD_NEVER_INDEXING
} nghttp2_hd_indexing_mode;

typedef struct {
  /* dynamic header table */
  nghttp2_hd_ringbuf hd_table;
  /* Memory allocator */
  nghttp2_mem *mem;
  /* Abstract buffer size of hd_table as described in the spec. This
     is the sum of length of name/value in hd_table +
     NGHTTP2_HD_ENTRY_OVERHEAD bytes overhead per each entry. */
  size_t hd_table_bufsize;
  /* The effective header table size. */
  size_t hd_table_bufsize_max;
  /* Next sequence number for nghttp2_hd_entry */
  uint32_t next_seq;
  /* If inflate/deflate error occurred, this value is set to 1 and
     further invocation of inflate/deflate will fail with
     NGHTTP2_ERR_HEADER_COMP. */
  uint8_t bad;
} nghttp2_hd_context;

#define HD_MAP_SIZE 128

typedef struct {
  nghttp2_hd_entry *table[HD_MAP_SIZE];
} nghttp2_hd_map;

struct nghttp2_hd_deflater {
  nghttp2_hd_context ctx;
  nghttp2_hd_map map;
  /* The upper limit of the header table size the deflater accepts. */
  size_t deflate_hd_table_bufsize_max;
  /* Minimum header table size notified in the next context update */
  size_t min_hd_table_bufsize_max;
  /* If nonzero, send header table size using encoding context update
     in the next deflate process */
  uint8_t notify_table_size_change;
};

struct nghttp2_hd_inflater {
  nghttp2_hd_context ctx;
  /* Stores current state of huffman decoding */
  nghttp2_hd_huff_decode_context huff_decode_ctx;
  /* header buffer */
  nghttp2_buf namebuf, valuebuf;
  nghttp2_rcbuf *namercbuf, *valuercbuf;
  /* Pointer to the name/value pair which are used in the current
     header emission. */
  nghttp2_rcbuf *nv_name_keep, *nv_value_keep;
  /* The number of bytes to read */
  size_t left;
  /* The index in indexed repr or indexed name */
  size_t index;
  /* The maximum header table size the inflater supports. This is the
     same value transmitted in SETTINGS_HEADER_TABLE_SIZE */
  size_t settings_hd_table_bufsize_max;
  /* Minimum header table size set by nghttp2_hd_inflate_change_table_size */
  size_t min_hd_table_bufsize_max;
  /* The number of next shift to decode integer */
  size_t shift;
  nghttp2_hd_opcode opcode;
  nghttp2_hd_inflate_state state;
  /* nonzero if string is huffman encoded */
  uint8_t huffman_encoded;
  /* nonzero if deflater requires that current entry is indexed */
  uint8_t index_required;
  /* nonzero if deflater requires that current entry must not be
     indexed */
  uint8_t no_index;
};

/*
 * Initializes the |ent| members.  The reference counts of nv->name
 * and nv->value are increased by one for each.
 */
void nghttp2_hd_entry_init(nghttp2_hd_entry *ent, nghttp2_hd_nv *nv);

/*
 * This function decreases the reference counts of nv->name and
 * nv->value.
 */
void nghttp2_hd_entry_free(nghttp2_hd_entry *ent);

/*
 * Initializes |deflater| for deflating name/values pairs.
 *
 * The encoder only uses up to
 * NGHTTP2_HD_DEFAULT_MAX_DEFLATE_BUFFER_SIZE bytes for header table
 * even if the larger value is specified later in
 * nghttp2_hd_change_table_size().
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * NGHTTP2_ERR_NOMEM
 *     Out of memory.
 */
int nghttp2_hd_deflate_init(nghttp2_hd_deflater *deflater, nghttp2_mem *mem);

/*
 * Initializes |deflater| for deflating name/values pairs.
 *
 * The encoder only uses up to |max_deflate_dynamic_table_size| bytes
 * for header table even if the larger value is specified later in
 * nghttp2_hd_change_table_size().
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * NGHTTP2_ERR_NOMEM
 *     Out of memory.
 */
int nghttp2_hd_deflate_init2(nghttp2_hd_deflater *deflater,
                             size_t max_deflate_dynamic_table_size,
                             nghttp2_mem *mem);

/*
 * Deallocates any resources allocated for |deflater|.
 */
void nghttp2_hd_deflate_free(nghttp2_hd_deflater *deflater);

/*
 * Deflates the |nva|, which has the |nvlen| name/value pairs, into
 * the |bufs|.
 *
 * This function expands |bufs| as necessary to store the result. If
 * buffers is full and the process still requires more space, this
 * function fails and returns NGHTTP2_ERR_HEADER_COMP.
 *
 * After this function returns, it is safe to delete the |nva|.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * NGHTTP2_ERR_NOMEM
 *     Out of memory.
 * NGHTTP2_ERR_HEADER_COMP
 *     Deflation process has failed.
 * NGHTTP2_ERR_BUFFER_ERROR
 *     Out of buffer space.
 */
int nghttp2_hd_deflate_hd_bufs(nghttp2_hd_deflater *deflater,
                               nghttp2_bufs *bufs, const nghttp2_nv *nva,
                               size_t nvlen);

/*
 * Initializes |inflater| for inflating name/values pairs.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :enum:`NGHTTP2_ERR_NOMEM`
 *     Out of memory.
 */
int nghttp2_hd_inflate_init(nghttp2_hd_inflater *inflater, nghttp2_mem *mem);

/*
 * Deallocates any resources allocated for |inflater|.
 */
void nghttp2_hd_inflate_free(nghttp2_hd_inflater *inflater);

/*
 * Similar to nghttp2_hd_inflate_hd(), but this takes nghttp2_hd_nv
 * instead of nghttp2_nv as output parameter |nv_out|.  Other than
 * that return values and semantics are the same as
 * nghttp2_hd_inflate_hd().
 */
ssize_t nghttp2_hd_inflate_hd_nv(nghttp2_hd_inflater *inflater,
                                 nghttp2_hd_nv *nv_out, int *inflate_flags,
                                 const uint8_t *in, size_t inlen, int in_final);

/* For unittesting purpose */
int nghttp2_hd_emit_indname_block(nghttp2_bufs *bufs, size_t index,
                                  nghttp2_nv *nv, int indexing_mode);

/* For unittesting purpose */
int nghttp2_hd_emit_newname_block(nghttp2_bufs *bufs, nghttp2_nv *nv,
                                  int indexing_mode);

/* For unittesting purpose */
int nghttp2_hd_emit_table_size(nghttp2_bufs *bufs, size_t table_size);

/* For unittesting purpose */
nghttp2_hd_nv nghttp2_hd_table_get(nghttp2_hd_context *context, size_t index);

/* For unittesting purpose */
ssize_t nghttp2_hd_decode_length(uint32_t *res, size_t *shift_ptr, int *fin,
                                 uint32_t initial, size_t shift, uint8_t *in,
                                 uint8_t *last, size_t prefix);

/* Huffman encoding/decoding functions */

/*
 * Counts the required bytes to encode |src| with length |len|.
 *
 * This function returns the number of required bytes to encode given
 * data, including padding of prefix of terminal symbol code. This
 * function always succeeds.
 */
size_t nghttp2_hd_huff_encode_count(const uint8_t *src, size_t len);

/*
 * Encodes the given data |src| with length |srclen| to the |bufs|.
 * This function expands extra buffers in |bufs| if necessary.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * NGHTTP2_ERR_NOMEM
 *     Out of memory.
 * NGHTTP2_ERR_BUFFER_ERROR
 *     Out of buffer space.
 */
int nghttp2_hd_huff_encode(nghttp2_bufs *bufs, const uint8_t *src,
                           size_t srclen);

void nghttp2_hd_huff_decode_context_init(nghttp2_hd_huff_decode_context *ctx);

/*
 * Decodes the given data |src| with length |srclen|.  The |ctx| must
 * be initialized by nghttp2_hd_huff_decode_context_init(). The result
 * will be written to |buf|.  This function assumes that |buf| has the
 * enough room to store the decoded byte string.
 *
 * The caller must set the |fin| to nonzero if the given input is the
 * final block.
 *
 * This function returns the number of read bytes from the |in|.
 *
 * If this function fails, it returns one of the following negative
 * return codes:
 *
 * NGHTTP2_ERR_NOMEM
 *     Out of memory.
 * NGHTTP2_ERR_HEADER_COMP
 *     Decoding process has failed.
 */
ssize_t nghttp2_hd_huff_decode(nghttp2_hd_huff_decode_context *ctx,
                               nghttp2_buf *buf, const uint8_t *src,
                               size_t srclen, int fin);

#endif /* NGHTTP2_HD_H */
