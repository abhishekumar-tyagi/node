/*
 * nghttp3
 *
 * Copyright (c) 2019 nghttp3 contributors
 * Copyright (c) 2017 ngtcp2 contributors
 * Copyright (c) 2012 nghttp2 contributors
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
#ifndef NGHTTP3_MAP_H
#define NGHTTP3_MAP_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <nghttp3/nghttp3.h>

#include "nghttp3_mem.h"

/* Implementation of unordered map */

typedef uint64_t nghttp3_map_key_type;

typedef struct nghttp3_map_bucket {
  uint32_t hash;
  nghttp3_map_key_type key;
  void *data;
} nghttp3_map_bucket;

typedef struct nghttp3_map {
  nghttp3_map_bucket *table;
  const nghttp3_mem *mem;
  size_t size;
  uint32_t tablelen;
  uint32_t tablelenbits;
} nghttp3_map;

/*
 * Initializes the map |map|.
 */
void nghttp3_map_init(nghttp3_map *map, const nghttp3_mem *mem);

/*
 * Deallocates any resources allocated for |map|. The stored entries
 * are not freed by this function. Use nghttp3_map_each_free() to free
 * each entries.
 */
void nghttp3_map_free(nghttp3_map *map);

/*
 * Deallocates each entries using |func| function and any resources
 * allocated for |map|. The |func| function is responsible for freeing
 * given the |data| object. The |ptr| will be passed to the |func| as
 * send argument. The return value of the |func| will be ignored.
 */
void nghttp3_map_each_free(nghttp3_map *map, int (*func)(void *data, void *ptr),
                           void *ptr);

/*
 * Inserts the new |data| with the |key| to the map |map|.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * NGHTTP3_ERR_INVALID_ARGUMENT
 *     The item associated by |key| already exists.
 * NGHTTP3_ERR_NOMEM
 *   Out of memory
 */
int nghttp3_map_insert(nghttp3_map *map, nghttp3_map_key_type key, void *data);

/*
 * Returns the data associated by the key |key|.  If there is no such
 * data, this function returns NULL.
 */
void *nghttp3_map_find(nghttp3_map *map, nghttp3_map_key_type key);

/*
 * Removes the data associated by the key |key| from the |map|.  The
 * removed data is not freed by this function.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * NGHTTP3_ERR_INVALID_ARGUMENT
 *     The data associated by |key| does not exist.
 */
int nghttp3_map_remove(nghttp3_map *map, nghttp3_map_key_type key);

/*
 * Removes all entries from |map|.
 */
void nghttp3_map_clear(nghttp3_map *map);

/*
 * Returns the number of items stored in the map |map|.
 */
size_t nghttp3_map_size(nghttp3_map *map);

/*
 * Applies the function |func| to each data in the |map| with the
 * optional user supplied pointer |ptr|.
 *
 * If the |func| returns 0, this function calls the |func| with the
 * next data.  If the |func| returns nonzero, it will not call the
 * |func| for further entries and return the return value of the
 * |func| immediately.  Thus, this function returns 0 if all the
 * invocations of the |func| return 0, or nonzero value which the last
 * invocation of |func| returns.
 *
 * Don't use this function to free each data. Use
 * nghttp3_map_each_free() instead.
 */
int nghttp3_map_each(nghttp3_map *map, int (*func)(void *data, void *ptr),
                     void *ptr);

#ifndef WIN32
void nghttp3_map_print_distance(nghttp3_map *map);
#endif /* !WIN32 */

#endif /* NGHTTP3_MAP_H */
