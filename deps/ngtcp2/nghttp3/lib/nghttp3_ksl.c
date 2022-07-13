/*
 * nghttp3
 *
 * Copyright (c) 2019 nghttp3 contributors
 * Copyright (c) 2018 ngtcp2 contributors
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
#include "nghttp3_ksl.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "nghttp3_macro.h"
#include "nghttp3_mem.h"
#include "nghttp3_range.h"

static size_t ksl_nodelen(size_t keylen) {
  return (sizeof(nghttp3_ksl_node) + keylen - sizeof(uint64_t) + 0xf) &
         (size_t)~0xf;
}

static size_t ksl_blklen(size_t nodelen) {
  return sizeof(nghttp3_ksl_blk) + nodelen * NGHTTP3_KSL_MAX_NBLK -
         sizeof(uint64_t);
}

/*
 * ksl_node_set_key sets |key| to |node|.
 */
static void ksl_node_set_key(nghttp3_ksl *ksl, nghttp3_ksl_node *node,
                             const void *key) {
  memcpy(node->key, key, ksl->keylen);
}

int nghttp3_ksl_init(nghttp3_ksl *ksl, nghttp3_ksl_compar compar, size_t keylen,
                     const nghttp3_mem *mem) {
  size_t nodelen = ksl_nodelen(keylen);
  size_t blklen = ksl_blklen(nodelen);
  nghttp3_ksl_blk *head;

  ksl->head = nghttp3_mem_malloc(mem, blklen);
  if (!ksl->head) {
    return NGHTTP3_ERR_NOMEM;
  }
  ksl->front = ksl->back = ksl->head;
  ksl->compar = compar;
  ksl->keylen = keylen;
  ksl->nodelen = nodelen;
  ksl->n = 0;
  ksl->mem = mem;

  head = ksl->head;
  head->next = head->prev = NULL;
  head->n = 0;
  head->leaf = 1;

  return 0;
}

/*
 * ksl_free_blk frees |blk| recursively.
 */
static void ksl_free_blk(nghttp3_ksl *ksl, nghttp3_ksl_blk *blk) {
  size_t i;

  if (!blk->leaf) {
    for (i = 0; i < blk->n; ++i) {
      ksl_free_blk(ksl, nghttp3_ksl_nth_node(ksl, blk, i)->blk);
    }
  }

  nghttp3_mem_free(ksl->mem, blk);
}

void nghttp3_ksl_free(nghttp3_ksl *ksl) {
  if (!ksl) {
    return;
  }

  ksl_free_blk(ksl, ksl->head);
}

/*
 * ksl_split_blk splits |blk| into 2 nghttp3_ksl_blk objects.  The new
 * nghttp3_ksl_blk is always the "right" block.
 *
 * It returns the pointer to the nghttp3_ksl_blk created which is the
 * located at the right of |blk|, or NULL which indicates out of
 * memory error.
 */
static nghttp3_ksl_blk *ksl_split_blk(nghttp3_ksl *ksl, nghttp3_ksl_blk *blk) {
  nghttp3_ksl_blk *rblk;

  rblk = nghttp3_mem_malloc(ksl->mem, ksl_blklen(ksl->nodelen));
  if (rblk == NULL) {
    return NULL;
  }

  rblk->next = blk->next;
  blk->next = rblk;
  if (rblk->next) {
    rblk->next->prev = rblk;
  } else if (ksl->back == blk) {
    ksl->back = rblk;
  }
  rblk->prev = blk;
  rblk->leaf = blk->leaf;

  rblk->n = blk->n / 2;

  memcpy(rblk->nodes, blk->nodes + ksl->nodelen * (blk->n - rblk->n),
         ksl->nodelen * rblk->n);

  blk->n -= rblk->n;

  assert(blk->n >= NGHTTP3_KSL_MIN_NBLK);
  assert(rblk->n >= NGHTTP3_KSL_MIN_NBLK);

  return rblk;
}

/*
 * ksl_split_node splits a node included in |blk| at the position |i|
 * into 2 adjacent nodes.  The new node is always inserted at the
 * position |i+1|.
 *
 * It returns 0 if it succeeds, or one of the following negative error
 * codes:
 *
 * NGHTTP3_ERR_NOMEM
 *   Out of memory.
 */
static int ksl_split_node(nghttp3_ksl *ksl, nghttp3_ksl_blk *blk, size_t i) {
  nghttp3_ksl_node *node;
  nghttp3_ksl_blk *lblk = nghttp3_ksl_nth_node(ksl, blk, i)->blk, *rblk;

  rblk = ksl_split_blk(ksl, lblk);
  if (rblk == NULL) {
    return NGHTTP3_ERR_NOMEM;
  }

  memmove(blk->nodes + (i + 2) * ksl->nodelen,
          blk->nodes + (i + 1) * ksl->nodelen,
          ksl->nodelen * (blk->n - (i + 1)));

  node = nghttp3_ksl_nth_node(ksl, blk, i + 1);
  node->blk = rblk;
  ++blk->n;
  ksl_node_set_key(ksl, node,
                   nghttp3_ksl_nth_node(ksl, rblk, rblk->n - 1)->key);

  node = nghttp3_ksl_nth_node(ksl, blk, i);
  ksl_node_set_key(ksl, node,
                   nghttp3_ksl_nth_node(ksl, lblk, lblk->n - 1)->key);

  return 0;
}

/*
 * ksl_split_head splits a head (root) block.  It increases the height
 * of skip list by 1.
 *
 * It returns 0 if it succeeds, or one of the following negative error
 * codes:
 *
 * NGHTTP3_ERR_NOMEM
 *   Out of memory.
 */
static int ksl_split_head(nghttp3_ksl *ksl) {
  nghttp3_ksl_blk *rblk = NULL, *lblk, *nhead = NULL;
  nghttp3_ksl_node *node;

  rblk = ksl_split_blk(ksl, ksl->head);
  if (rblk == NULL) {
    return NGHTTP3_ERR_NOMEM;
  }

  lblk = ksl->head;

  nhead = nghttp3_mem_malloc(ksl->mem, ksl_blklen(ksl->nodelen));
  if (nhead == NULL) {
    nghttp3_mem_free(ksl->mem, rblk);
    return NGHTTP3_ERR_NOMEM;
  }
  nhead->next = nhead->prev = NULL;
  nhead->n = 2;
  nhead->leaf = 0;

  node = nghttp3_ksl_nth_node(ksl, nhead, 0);
  ksl_node_set_key(ksl, node,
                   nghttp3_ksl_nth_node(ksl, lblk, lblk->n - 1)->key);
  node->blk = lblk;

  node = nghttp3_ksl_nth_node(ksl, nhead, 1);
  ksl_node_set_key(ksl, node,
                   nghttp3_ksl_nth_node(ksl, rblk, rblk->n - 1)->key);
  node->blk = rblk;

  ksl->head = nhead;

  return 0;
}

/*
 * insert_node inserts a node whose key is |key| with the associated
 * |data| at the index of |i|.  This function assumes that the number
 * of nodes contained by |blk| is strictly less than
 * NGHTTP3_KSL_MAX_NBLK.
 */
static void ksl_insert_node(nghttp3_ksl *ksl, nghttp3_ksl_blk *blk, size_t i,
                            const nghttp3_ksl_key *key, void *data) {
  nghttp3_ksl_node *node;

  assert(blk->n < NGHTTP3_KSL_MAX_NBLK);

  memmove(blk->nodes + (i + 1) * ksl->nodelen, blk->nodes + i * ksl->nodelen,
          ksl->nodelen * (blk->n - i));

  node = nghttp3_ksl_nth_node(ksl, blk, i);
  ksl_node_set_key(ksl, node, key);
  node->data = data;

  ++blk->n;
}

static size_t ksl_bsearch(nghttp3_ksl *ksl, nghttp3_ksl_blk *blk,
                          const nghttp3_ksl_key *key,
                          nghttp3_ksl_compar compar) {
  nghttp3_ssize left = -1, right = (nghttp3_ssize)blk->n, mid;
  nghttp3_ksl_node *node;

  while (right - left > 1) {
    mid = (left + right) >> 1;
    node = nghttp3_ksl_nth_node(ksl, blk, (size_t)mid);
    if (compar((nghttp3_ksl_key *)node->key, key)) {
      left = mid;
    } else {
      right = mid;
    }
  }

  return (size_t)right;
}

int nghttp3_ksl_insert(nghttp3_ksl *ksl, nghttp3_ksl_it *it,
                       const nghttp3_ksl_key *key, void *data) {
  nghttp3_ksl_blk *blk = ksl->head;
  nghttp3_ksl_node *node;
  size_t i;
  int rv;

  if (blk->n == NGHTTP3_KSL_MAX_NBLK) {
    rv = ksl_split_head(ksl);
    if (rv != 0) {
      return rv;
    }
    blk = ksl->head;
  }

  for (;;) {
    i = ksl_bsearch(ksl, blk, key, ksl->compar);

    if (blk->leaf) {
      if (i < blk->n &&
          !ksl->compar(key, nghttp3_ksl_nth_node(ksl, blk, i)->key)) {
        if (it) {
          *it = nghttp3_ksl_end(ksl);
        }
        return NGHTTP3_ERR_INVALID_ARGUMENT;
      }
      ksl_insert_node(ksl, blk, i, key, data);
      ++ksl->n;
      if (it) {
        nghttp3_ksl_it_init(it, ksl, blk, i);
      }
      return 0;
    }

    if (i == blk->n) {
      /* This insertion extends the largest key in this subtree. */
      for (; !blk->leaf;) {
        node = nghttp3_ksl_nth_node(ksl, blk, blk->n - 1);
        if (node->blk->n == NGHTTP3_KSL_MAX_NBLK) {
          rv = ksl_split_node(ksl, blk, blk->n - 1);
          if (rv != 0) {
            return rv;
          }
          node = nghttp3_ksl_nth_node(ksl, blk, blk->n - 1);
        }
        ksl_node_set_key(ksl, node, key);
        blk = node->blk;
      }
      ksl_insert_node(ksl, blk, blk->n, key, data);
      ++ksl->n;
      if (it) {
        nghttp3_ksl_it_init(it, ksl, blk, blk->n - 1);
      }
      return 0;
    }

    node = nghttp3_ksl_nth_node(ksl, blk, i);

    if (node->blk->n == NGHTTP3_KSL_MAX_NBLK) {
      rv = ksl_split_node(ksl, blk, i);
      if (rv != 0) {
        return rv;
      }
      if (ksl->compar((nghttp3_ksl_key *)node->key, key)) {
        node = nghttp3_ksl_nth_node(ksl, blk, i + 1);
        if (ksl->compar((nghttp3_ksl_key *)node->key, key)) {
          ksl_node_set_key(ksl, node, key);
        }
      }
    }

    blk = node->blk;
  }
}

/*
 * ksl_remove_node removes the node included in |blk| at the index of
 * |i|.
 */
static void ksl_remove_node(nghttp3_ksl *ksl, nghttp3_ksl_blk *blk, size_t i) {
  memmove(blk->nodes + i * ksl->nodelen, blk->nodes + (i + 1) * ksl->nodelen,
          ksl->nodelen * (blk->n - (i + 1)));

  --blk->n;
}

/*
 * ksl_merge_node merges 2 nodes which are the nodes at the index of
 * |i| and |i + 1|.
 *
 * If |blk| is the direct descendant of head (root) block and the head
 * block contains just 2 nodes, the merged block becomes head block,
 * which decreases the height of |ksl| by 1.
 *
 * This function returns the pointer to the merged block.
 */
static nghttp3_ksl_blk *ksl_merge_node(nghttp3_ksl *ksl, nghttp3_ksl_blk *blk,
                                       size_t i) {
  nghttp3_ksl_blk *lblk, *rblk;

  assert(i + 1 < blk->n);

  lblk = nghttp3_ksl_nth_node(ksl, blk, i)->blk;
  rblk = nghttp3_ksl_nth_node(ksl, blk, i + 1)->blk;

  assert(lblk->n + rblk->n < NGHTTP3_KSL_MAX_NBLK);

  memcpy(lblk->nodes + ksl->nodelen * lblk->n, rblk->nodes,
         ksl->nodelen * rblk->n);

  lblk->n += rblk->n;
  lblk->next = rblk->next;
  if (lblk->next) {
    lblk->next->prev = lblk;
  } else if (ksl->back == rblk) {
    ksl->back = lblk;
  }

  nghttp3_mem_free(ksl->mem, rblk);

  if (ksl->head == blk && blk->n == 2) {
    nghttp3_mem_free(ksl->mem, ksl->head);
    ksl->head = lblk;
  } else {
    ksl_remove_node(ksl, blk, i + 1);
    ksl_node_set_key(ksl, nghttp3_ksl_nth_node(ksl, blk, i),
                     nghttp3_ksl_nth_node(ksl, lblk, lblk->n - 1)->key);
  }

  return lblk;
}

/*
 * ksl_shift_left moves the first nodes in blk->nodes[i]->blk->nodes
 * to blk->nodes[i - 1]->blk->nodes in a manner that they have the
 * same amount of nodes as much as possible.
 */
static void ksl_shift_left(nghttp3_ksl *ksl, nghttp3_ksl_blk *blk, size_t i) {
  nghttp3_ksl_node *lnode, *rnode;
  size_t n;

  assert(i > 0);

  lnode = nghttp3_ksl_nth_node(ksl, blk, i - 1);
  rnode = nghttp3_ksl_nth_node(ksl, blk, i);

  assert(lnode->blk->n < NGHTTP3_KSL_MAX_NBLK);
  assert(rnode->blk->n > NGHTTP3_KSL_MIN_NBLK);

  n = (lnode->blk->n + rnode->blk->n + 1) / 2 - lnode->blk->n;

  assert(n > 0);
  assert(lnode->blk->n <= NGHTTP3_KSL_MAX_NBLK - n);
  assert(rnode->blk->n >= NGHTTP3_KSL_MIN_NBLK + n);

  memcpy(lnode->blk->nodes + ksl->nodelen * lnode->blk->n, rnode->blk->nodes,
         ksl->nodelen * n);

  lnode->blk->n += (uint32_t)n;
  rnode->blk->n -= (uint32_t)n;

  ksl_node_set_key(
      ksl, lnode,
      nghttp3_ksl_nth_node(ksl, lnode->blk, lnode->blk->n - 1)->key);

  memmove(rnode->blk->nodes, rnode->blk->nodes + ksl->nodelen * n,
          ksl->nodelen * rnode->blk->n);
}

/*
 * ksl_shift_right moves the last nodes in blk->nodes[i]->blk->nodes
 * to blk->nodes[i + 1]->blk->nodes in a manner that they have the
 * same amount of nodes as much as possible..
 */
static void ksl_shift_right(nghttp3_ksl *ksl, nghttp3_ksl_blk *blk, size_t i) {
  nghttp3_ksl_node *lnode, *rnode;
  size_t n;

  assert(i < blk->n - 1);

  lnode = nghttp3_ksl_nth_node(ksl, blk, i);
  rnode = nghttp3_ksl_nth_node(ksl, blk, i + 1);

  assert(lnode->blk->n > NGHTTP3_KSL_MIN_NBLK);
  assert(rnode->blk->n < NGHTTP3_KSL_MAX_NBLK);

  n = (lnode->blk->n + rnode->blk->n + 1) / 2 - rnode->blk->n;

  assert(n > 0);
  assert(lnode->blk->n >= NGHTTP3_KSL_MIN_NBLK + n);
  assert(rnode->blk->n <= NGHTTP3_KSL_MAX_NBLK - n);

  memmove(rnode->blk->nodes + ksl->nodelen * n, rnode->blk->nodes,
          ksl->nodelen * rnode->blk->n);

  rnode->blk->n += (uint32_t)n;
  lnode->blk->n -= (uint32_t)n;

  memcpy(rnode->blk->nodes, lnode->blk->nodes + ksl->nodelen * lnode->blk->n,
         ksl->nodelen * n);

  ksl_node_set_key(
      ksl, lnode,
      nghttp3_ksl_nth_node(ksl, lnode->blk, lnode->blk->n - 1)->key);
}

/*
 * key_equal returns nonzero if |lhs| and |rhs| are equal using the
 * function |compar|.
 */
static int key_equal(nghttp3_ksl_compar compar, const nghttp3_ksl_key *lhs,
                     const nghttp3_ksl_key *rhs) {
  return !compar(lhs, rhs) && !compar(rhs, lhs);
}

int nghttp3_ksl_remove(nghttp3_ksl *ksl, nghttp3_ksl_it *it,
                       const nghttp3_ksl_key *key) {
  nghttp3_ksl_blk *blk = ksl->head;
  nghttp3_ksl_node *node;
  size_t i;

  if (!blk->leaf && blk->n == 2 &&
      nghttp3_ksl_nth_node(ksl, blk, 0)->blk->n == NGHTTP3_KSL_MIN_NBLK &&
      nghttp3_ksl_nth_node(ksl, blk, 1)->blk->n == NGHTTP3_KSL_MIN_NBLK) {
    blk = ksl_merge_node(ksl, ksl->head, 0);
  }

  for (;;) {
    i = ksl_bsearch(ksl, blk, key, ksl->compar);

    if (i == blk->n) {
      if (it) {
        *it = nghttp3_ksl_end(ksl);
      }
      return NGHTTP3_ERR_INVALID_ARGUMENT;
    }

    if (blk->leaf) {
      if (ksl->compar(key, nghttp3_ksl_nth_node(ksl, blk, i)->key)) {
        if (it) {
          *it = nghttp3_ksl_end(ksl);
        }
        return NGHTTP3_ERR_INVALID_ARGUMENT;
      }
      ksl_remove_node(ksl, blk, i);
      --ksl->n;
      if (it) {
        if (blk->n == i && blk->next) {
          nghttp3_ksl_it_init(it, ksl, blk->next, 0);
        } else {
          nghttp3_ksl_it_init(it, ksl, blk, i);
        }
      }
      return 0;
    }

    node = nghttp3_ksl_nth_node(ksl, blk, i);

    if (node->blk->n > NGHTTP3_KSL_MIN_NBLK) {
      blk = node->blk;
      continue;
    }

    assert(node->blk->n == NGHTTP3_KSL_MIN_NBLK);

    if (i + 1 < blk->n &&
        nghttp3_ksl_nth_node(ksl, blk, i + 1)->blk->n > NGHTTP3_KSL_MIN_NBLK) {
      ksl_shift_left(ksl, blk, i + 1);
      blk = node->blk;
      continue;
    }

    if (i > 0 &&
        nghttp3_ksl_nth_node(ksl, blk, i - 1)->blk->n > NGHTTP3_KSL_MIN_NBLK) {
      ksl_shift_right(ksl, blk, i - 1);
      blk = node->blk;
      continue;
    }

    if (i + 1 < blk->n) {
      blk = ksl_merge_node(ksl, blk, i);
      continue;
    }

    assert(i > 0);

    blk = ksl_merge_node(ksl, blk, i - 1);
  }
}

nghttp3_ksl_it nghttp3_ksl_lower_bound(nghttp3_ksl *ksl,
                                       const nghttp3_ksl_key *key) {
  nghttp3_ksl_blk *blk = ksl->head;
  nghttp3_ksl_it it;
  size_t i;

  for (;;) {
    i = ksl_bsearch(ksl, blk, key, ksl->compar);

    if (blk->leaf) {
      if (i == blk->n && blk->next) {
        blk = blk->next;
        i = 0;
      }
      nghttp3_ksl_it_init(&it, ksl, blk, i);
      return it;
    }

    if (i == blk->n) {
      /* This happens if descendant has smaller key.  Fast forward to
         find last node in this subtree. */
      for (; !blk->leaf; blk = nghttp3_ksl_nth_node(ksl, blk, blk->n - 1)->blk)
        ;
      if (blk->next) {
        blk = blk->next;
        i = 0;
      } else {
        i = blk->n;
      }
      nghttp3_ksl_it_init(&it, ksl, blk, i);
      return it;
    }
    blk = nghttp3_ksl_nth_node(ksl, blk, i)->blk;
  }
}

nghttp3_ksl_it nghttp3_ksl_lower_bound_compar(nghttp3_ksl *ksl,
                                              const nghttp3_ksl_key *key,
                                              nghttp3_ksl_compar compar) {
  nghttp3_ksl_blk *blk = ksl->head;
  nghttp3_ksl_it it;
  size_t i;

  for (;;) {
    i = ksl_bsearch(ksl, blk, key, compar);

    if (blk->leaf) {
      if (i == blk->n && blk->next) {
        blk = blk->next;
        i = 0;
      }
      nghttp3_ksl_it_init(&it, ksl, blk, i);
      return it;
    }

    if (i == blk->n) {
      /* This happens if descendant has smaller key.  Fast forward to
         find last node in this subtree. */
      for (; !blk->leaf; blk = nghttp3_ksl_nth_node(ksl, blk, blk->n - 1)->blk)
        ;
      if (blk->next) {
        blk = blk->next;
        i = 0;
      } else {
        i = blk->n;
      }
      nghttp3_ksl_it_init(&it, ksl, blk, i);
      return it;
    }
    blk = nghttp3_ksl_nth_node(ksl, blk, i)->blk;
  }
}

void nghttp3_ksl_update_key(nghttp3_ksl *ksl, const nghttp3_ksl_key *old_key,
                            const nghttp3_ksl_key *new_key) {
  nghttp3_ksl_blk *blk = ksl->head;
  nghttp3_ksl_node *node;
  size_t i;

  for (;;) {
    i = ksl_bsearch(ksl, blk, old_key, ksl->compar);

    assert(i < blk->n);
    node = nghttp3_ksl_nth_node(ksl, blk, i);

    if (blk->leaf) {
      assert(key_equal(ksl->compar, (nghttp3_ksl_key *)node->key, old_key));
      ksl_node_set_key(ksl, node, new_key);
      return;
    }

    if (key_equal(ksl->compar, (nghttp3_ksl_key *)node->key, old_key) ||
        ksl->compar((nghttp3_ksl_key *)node->key, new_key)) {
      ksl_node_set_key(ksl, node, new_key);
    }

    blk = node->blk;
  }
}

static void ksl_print(nghttp3_ksl *ksl, nghttp3_ksl_blk *blk, size_t level) {
  size_t i;
  nghttp3_ksl_node *node;

  fprintf(stderr, "LV=%zu n=%u\n", level, blk->n);

  if (blk->leaf) {
    for (i = 0; i < blk->n; ++i) {
      node = nghttp3_ksl_nth_node(ksl, blk, i);
      fprintf(stderr, " %" PRId64, *(int64_t *)(void *)node->key);
    }
    fprintf(stderr, "\n");
    return;
  }

  for (i = 0; i < blk->n; ++i) {
    ksl_print(ksl, nghttp3_ksl_nth_node(ksl, blk, i)->blk, level + 1);
  }
}

size_t nghttp3_ksl_len(nghttp3_ksl *ksl) { return ksl->n; }

void nghttp3_ksl_clear(nghttp3_ksl *ksl) {
  size_t i;
  nghttp3_ksl_blk *head;

  if (!ksl->head->leaf) {
    for (i = 0; i < ksl->head->n; ++i) {
      ksl_free_blk(ksl, nghttp3_ksl_nth_node(ksl, ksl->head, i)->blk);
    }
  }

  ksl->front = ksl->back = ksl->head;
  ksl->n = 0;

  head = ksl->head;

  head->next = head->prev = NULL;
  head->n = 0;
  head->leaf = 1;
}

void nghttp3_ksl_print(nghttp3_ksl *ksl) { ksl_print(ksl, ksl->head, 0); }

nghttp3_ksl_it nghttp3_ksl_begin(const nghttp3_ksl *ksl) {
  nghttp3_ksl_it it;
  nghttp3_ksl_it_init(&it, ksl, ksl->front, 0);
  return it;
}

nghttp3_ksl_it nghttp3_ksl_end(const nghttp3_ksl *ksl) {
  nghttp3_ksl_it it;
  nghttp3_ksl_it_init(&it, ksl, ksl->back, ksl->back->n);
  return it;
}

void nghttp3_ksl_it_init(nghttp3_ksl_it *it, const nghttp3_ksl *ksl,
                         nghttp3_ksl_blk *blk, size_t i) {
  it->ksl = ksl;
  it->blk = blk;
  it->i = i;
}

void *nghttp3_ksl_it_get(const nghttp3_ksl_it *it) {
  assert(it->i < it->blk->n);
  return nghttp3_ksl_nth_node(it->ksl, it->blk, it->i)->data;
}

void nghttp3_ksl_it_prev(nghttp3_ksl_it *it) {
  assert(!nghttp3_ksl_it_begin(it));

  if (it->i == 0) {
    it->blk = it->blk->prev;
    it->i = it->blk->n - 1;
  } else {
    --it->i;
  }
}

int nghttp3_ksl_it_begin(const nghttp3_ksl_it *it) {
  return it->i == 0 && it->blk->prev == NULL;
}

int nghttp3_ksl_range_compar(const nghttp3_ksl_key *lhs,
                             const nghttp3_ksl_key *rhs) {
  const nghttp3_range *a = lhs, *b = rhs;
  return a->begin < b->begin;
}

int nghttp3_ksl_range_exclusive_compar(const nghttp3_ksl_key *lhs,
                                       const nghttp3_ksl_key *rhs) {
  const nghttp3_range *a = lhs, *b = rhs;
  return a->begin < b->begin &&
         !(nghttp3_max(a->begin, b->begin) < nghttp3_min(a->end, b->end));
}
