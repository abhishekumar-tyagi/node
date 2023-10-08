/* MIT License
 *
 * Copyright (c) 2023 Brad House
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 */
#include "ares_setup.h"
#include "ares.h"
#include "ares_private.h"
#include "ares__llist.h"
#include "ares__htable.h"

#define ARES__HTABLE_MAX_BUCKETS (1U<<24)
#define ARES__HTABLE_MIN_BUCKETS (1U<<4)
#define ARES__HTABLE_EXPAND_PERCENT 75

struct ares__htable {
  ares__htable_hashfunc_t    hash;
  ares__htable_bucket_key_t  bucket_key;
  ares__htable_bucket_free_t bucket_free;
  ares__htable_key_eq_t      key_eq;
  unsigned int               seed;
  unsigned int               size;
  size_t                     num_keys;
  /* NOTE: if we converted buckets into ares__slist_t we could guarantee on
   *       hash collisions we would have O(log n) worst case insert and search
   *       performance.  (We'd also need to make key_eq into a key_cmp to
   *       support sort).  That said, risk with a random hash seed is near zero,
   *       and ares__slist_t is heavier weight so I think using ares__llist_t is
   *       is an overall win. */
  ares__llist_t            **buckets;
};


static unsigned int ares__htable_generate_seed(ares__htable_t *htable)
{
  unsigned int seed = 0;

  /* Mix stack address, heap address, and time to generate a random seed, it
   * doesn't have to be super secure, just quick.  Likelihood of a hash
   * collision attack is very low with a small amount of effort */
  seed |= (unsigned int)((size_t)htable & 0xFFFFFFFF);
  seed |= (unsigned int)((size_t)&seed & 0xFFFFFFFF);
  seed |= (unsigned int)time(NULL) & 0xFFFFFFFF;
  return seed;
}

static void ares__htable_buckets_destroy(ares__llist_t **buckets,
                                         unsigned int size,
                                         unsigned char destroy_vals)
{
  unsigned int i;

  if (buckets == NULL)
    return;

  for (i=0; i<size; i++) {
    if (buckets[i] == NULL)
      continue;

    if (!destroy_vals)
      ares__llist_replace_destructor(buckets[i], NULL);

    ares__llist_destroy(buckets[i]);
  }

  ares_free(buckets);
}


void ares__htable_destroy(ares__htable_t *htable)
{
  if (htable == NULL)
    return;
  ares__htable_buckets_destroy(htable->buckets, htable->size, 1);
  ares_free(htable);
}


ares__htable_t *ares__htable_create(ares__htable_hashfunc_t    hash_func,
                                    ares__htable_bucket_key_t  bucket_key,
                                    ares__htable_bucket_free_t bucket_free,
                                    ares__htable_key_eq_t      key_eq)
{
  ares__htable_t *htable = NULL;

  if (hash_func == NULL || bucket_key == NULL || bucket_free == NULL ||
      key_eq == NULL) {
    goto fail;
  }

  htable = ares_malloc(sizeof(*htable));
  if (htable == NULL)
    goto fail;

  memset(htable, 0, sizeof(*htable));

  htable->hash        = hash_func;
  htable->bucket_key  = bucket_key;
  htable->bucket_free = bucket_free;
  htable->key_eq      = key_eq;
  htable->seed        = ares__htable_generate_seed(htable);
  htable->size        = ARES__HTABLE_MIN_BUCKETS;
  htable->buckets     = ares_malloc(sizeof(*htable->buckets) * htable->size);

  if (htable->buckets == NULL)
    goto fail;

  memset(htable->buckets, 0, sizeof(*htable->buckets) * htable->size);

  return htable;

fail:
  ares__htable_destroy(htable);
  return NULL;
}


/*! Grabs the Hashtable index from the key and length.  The h index is
 *  the hash of the function reduced to the size of the bucket list.
 *  We are doing "hash & (size - 1)" since we are guaranteeing a power of
 *  2 for size. This is equivalent to "hash % size", but should be more
 * efficient */
#define HASH_IDX(h, key) h->hash(key, h->seed) & (h->size - 1)

static ares__llist_node_t *ares__htable_find(ares__htable_t *htable,
                                             unsigned int idx,
                                             const void *key)
{
  ares__llist_node_t *node = NULL;

  for (node = ares__llist_node_first(htable->buckets[idx]);
       node != NULL;
       node = ares__llist_node_next(node)) {

    if (htable->key_eq(key, htable->bucket_key(ares__llist_node_val(node))))
      break;
  }

  return node;
}


static unsigned int ares__htable_expand(ares__htable_t *htable)
{
  ares__llist_t **buckets  = NULL;
  unsigned int    old_size = htable->size;
  size_t          i;

  /* Not a failure, just won't expand */
  if (old_size == ARES__HTABLE_MAX_BUCKETS)
    return 1;

  htable->size <<= 1;

  /* We must do this in 2 passes as we want it to be non-destructive in case
   * there is a memory allocation failure.  So we will actually use more 
   * memory doing it this way, but at least we might be able to gracefully
   * recover */
  buckets = ares_malloc(sizeof(*buckets) * htable->size);
  if (buckets == NULL)
    goto fail;

  memset(buckets, 0, sizeof(*buckets) * htable->size);

  for (i=0; i<old_size; i++) {
    ares__llist_node_t *node;
    for (node = ares__llist_node_first(htable->buckets[i]);
         node != NULL;
         node = ares__llist_node_next(node)) {

      void  *val = ares__llist_node_val(node);
      size_t idx = HASH_IDX(htable, htable->bucket_key(val));

      if (buckets[idx] == NULL) {
        buckets[idx] = ares__llist_create(htable->bucket_free);
        if (buckets[idx] == NULL)
          goto fail;
      }

      if (ares__llist_insert_first(buckets[idx], val) == NULL) {
        goto fail;
      }

    }
  }

  /* Swap out buckets */
  ares__htable_buckets_destroy(htable->buckets, old_size, 0);
  htable->buckets = buckets;
  return 1;

fail:
  ares__htable_buckets_destroy(buckets, htable->size, 0);
  htable->size = old_size;

  return 0;
}


unsigned int ares__htable_insert(ares__htable_t *htable, void *bucket)
{
  unsigned int        idx  = 0;
  ares__llist_node_t *node = NULL;
  const void         *key  = NULL;

  if (htable == NULL || bucket == NULL)
    return 0;


  key  = htable->bucket_key(bucket);
  idx  = HASH_IDX(htable, key);

  /* See if we have a matching bucket already, if so, replace it */
  node = ares__htable_find(htable, idx, key);
  if (node != NULL) {
    ares__llist_node_replace(node, bucket);
    return 1;
  }

  /* Check to see if we should rehash because likelihood of collisions has
   * increased beyond our threshold */
  if (htable->num_keys+1 > (htable->size * ARES__HTABLE_EXPAND_PERCENT) / 100) {
    if (!ares__htable_expand(htable)) {
      return 0;
    }
    /* If we expanded, need to calculate a new index */
    idx = HASH_IDX(htable, key);
  }

  /* We lazily allocate the linked list */
  if (htable->buckets[idx] == NULL) {
    htable->buckets[idx] = ares__llist_create(htable->bucket_free);
    if (htable->buckets[idx] == NULL)
      return 0;
  }
  
  node = ares__llist_insert_first(htable->buckets[idx], bucket);
  if (node == NULL)
    return 0;

  htable->num_keys++;

  return 1;
}

  
void *ares__htable_get(ares__htable_t *htable, const void *key)
{
  unsigned int idx;

  if (htable == NULL || key == NULL)
    return NULL;

  idx = HASH_IDX(htable, key);

  return ares__llist_node_val(ares__htable_find(htable, idx, key));
}


unsigned int ares__htable_remove(ares__htable_t *htable, const void *key)
{
  ares__llist_node_t *node;
  unsigned int        idx;

  if (htable == NULL || key == NULL)
    return 0;

  idx  = HASH_IDX(htable, key);
  node = ares__htable_find(htable, idx, key);
  if (node == NULL)
    return 0;

  htable->num_keys--;
  ares__llist_node_destroy(node);
  return 1;
}

size_t ares__htable_num_keys(ares__htable_t *htable)
{
  if (htable == NULL)
    return 0;
  return htable->num_keys;
}

unsigned int ares__htable_hash_FNV1a(const unsigned char *key, size_t key_len,
                                     unsigned int seed)
{
  /* recommended seed is 2166136261U, but we don't want collisions */
  unsigned int         hv   = seed; 
  size_t               i;

  for (i = 0; i < key_len; i++) {
    hv ^= (unsigned int)key[i];
    /* hv *= 0x01000193 */
    hv += (hv<<1) + (hv<<4) + (hv<<7) + (hv<<8) + (hv<<24);
  }

  return hv;
}

/* tolower() is locale-specific.  Use a lookup table fast conversion that only
 * operates on ASCII */
static const unsigned char ares__tolower_lookup[] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
  0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
  0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
  0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
  0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
  0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
  0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
  0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
  0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
  0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
  0x78,0x79,0x7A,0x5B,0x5C,0x5D,0x5E,0x5F,
  0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
  0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
  0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
  0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
  0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
  0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
  0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
  0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
  0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
  0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
  0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
  0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
  0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
  0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
  0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
  0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
  0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};


/* Case insensitive version, meant for ASCII strings */
unsigned int ares__htable_hash_FNV1a_casecmp(const unsigned char *key, size_t key_len,
                                             unsigned int seed)
{
  /* recommended seed is 2166136261U, but we don't want collisions */
  unsigned int         hv   = seed;
  size_t               i;

  for (i = 0; i < key_len; i++) {
    hv ^= (unsigned int)ares__tolower_lookup[key[i]];
    /* hv *= 0x01000193 */
    hv += (hv<<1) + (hv<<4) + (hv<<7) + (hv<<8) + (hv<<24);
  }

  return hv;
}
