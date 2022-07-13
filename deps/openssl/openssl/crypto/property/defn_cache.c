/*
 * Copyright 2019-2022 The OpenSSL Project Authors. All Rights Reserved.
 * Copyright (c) 2019, Oracle and/or its affiliates.  All rights reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <string.h>
#include <openssl/err.h>
#include <openssl/lhash.h>
#include "internal/propertyerr.h"
#include "internal/property.h"
#include "internal/core.h"
#include "property_local.h"

/*
 * Implement a property definition cache.
 * These functions assume that they are called under a write lock.
 * No attempt is made to clean out the cache, except when it is shut down.
 */

typedef struct {
    const char *prop;
    OSSL_PROPERTY_LIST *defn;
    char body[1];
} PROPERTY_DEFN_ELEM;

DEFINE_LHASH_OF(PROPERTY_DEFN_ELEM);

static unsigned long property_defn_hash(const PROPERTY_DEFN_ELEM *a)
{
    return OPENSSL_LH_strhash(a->prop);
}

static int property_defn_cmp(const PROPERTY_DEFN_ELEM *a,
                             const PROPERTY_DEFN_ELEM *b)
{
    return strcmp(a->prop, b->prop);
}

static void property_defn_free(PROPERTY_DEFN_ELEM *elem)
{
    ossl_property_free(elem->defn);
    OPENSSL_free(elem);
}

static void property_defns_free(void *vproperty_defns)
{
    LHASH_OF(PROPERTY_DEFN_ELEM) *property_defns = vproperty_defns;

    if (property_defns != NULL) {
        lh_PROPERTY_DEFN_ELEM_doall(property_defns,
                                    &property_defn_free);
        lh_PROPERTY_DEFN_ELEM_free(property_defns);
    }
}

static void *property_defns_new(OSSL_LIB_CTX *ctx) {
    return lh_PROPERTY_DEFN_ELEM_new(&property_defn_hash, &property_defn_cmp);
}

static const OSSL_LIB_CTX_METHOD property_defns_method = {
    OSSL_LIB_CTX_METHOD_DEFAULT_PRIORITY,
    property_defns_new,
    property_defns_free,
};

OSSL_PROPERTY_LIST *ossl_prop_defn_get(OSSL_LIB_CTX *ctx, const char *prop)
{
    PROPERTY_DEFN_ELEM elem, *r;
    LHASH_OF(PROPERTY_DEFN_ELEM) *property_defns;

    property_defns = ossl_lib_ctx_get_data(ctx,
                                           OSSL_LIB_CTX_PROPERTY_DEFN_INDEX,
                                           &property_defns_method);
    if (property_defns == NULL || !ossl_lib_ctx_read_lock(ctx))
        return NULL;

    elem.prop = prop;
    r = lh_PROPERTY_DEFN_ELEM_retrieve(property_defns, &elem);
    ossl_lib_ctx_unlock(ctx);
    return r != NULL ? r->defn : NULL;
}

/*
 * Cache the property list for a given property string. Callers of this function
 * should call ossl_prop_defn_get first to ensure that there is no existing
 * cache entry for this property string.
 */
int ossl_prop_defn_set(OSSL_LIB_CTX *ctx, const char *prop,
                       OSSL_PROPERTY_LIST *pl)
{
    PROPERTY_DEFN_ELEM elem, *old, *p = NULL;
    size_t len;
    LHASH_OF(PROPERTY_DEFN_ELEM) *property_defns;
    int res = 1;

    property_defns = ossl_lib_ctx_get_data(ctx,
                                           OSSL_LIB_CTX_PROPERTY_DEFN_INDEX,
                                           &property_defns_method);
    if (property_defns == NULL)
        return 0;

    if (prop == NULL)
        return 1;

    if (!ossl_lib_ctx_write_lock(ctx))
        return 0;
    if (pl == NULL) {
        elem.prop = prop;
        lh_PROPERTY_DEFN_ELEM_delete(property_defns, &elem);
        goto end;
    }
    len = strlen(prop);
    p = OPENSSL_malloc(sizeof(*p) + len);
    if (p != NULL) {
        p->prop = p->body;
        p->defn = pl;
        memcpy(p->body, prop, len + 1);
        old = lh_PROPERTY_DEFN_ELEM_insert(property_defns, p);
        if (!ossl_assert(old == NULL)) {
            /*
             * This should not happen. Any caller of ossl_prop_defn_set should
             * have called ossl_prop_defn_get first - so we should know that
             * there is no existing entry. If we get here we have a bug. We
             * deliberately leak the |old| reference in order to avoid a crash
             * if there are any existing users of it.
             */
            goto end;
        }
        if (!lh_PROPERTY_DEFN_ELEM_error(property_defns))
            goto end;
    }
    OPENSSL_free(p);
    res = 0;
 end:
    ossl_lib_ctx_unlock(ctx);
    return res;
}
