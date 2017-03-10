/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/* ====================================================================
 * Copyright 2005 Nokia. All rights reserved.
 *
 * The portions of the attached software ("Contribution") is developed by
 * Nokia Corporation and is licensed pursuant to the OpenSSL open source
 * license.
 *
 * The Contribution, originally written by Mika Kousa and Pasi Eronen of
 * Nokia Corporation, consists of the "PSK" (Pre-Shared Key) ciphersuites
 * support (see RFC 4279) to OpenSSL.
 *
 * No patent licenses or other rights except those expressly stated in
 * the OpenSSL open source license shall be deemed granted or received
 * expressly, by implication, estoppel, or otherwise.
 *
 * No assurances are provided by Nokia that the Contribution does not
 * infringe the patent or other intellectual property rights of any third
 * party or that the license provides you with all the necessary rights
 * to make use of the Contribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. IN
 * ADDITION TO THE DISCLAIMERS INCLUDED IN THE LICENSE, NOKIA
 * SPECIFICALLY DISCLAIMS ANY LIABILITY FOR CLAIMS BROUGHT BY YOU OR ANY
 * OTHER ENTITY BASED ON INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS OR
 * OTHERWISE.
 */

#include <stdio.h>
#include <openssl/lhash.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include "ssl_locl.h"

static void SSL_SESSION_list_remove(SSL_CTX *ctx, SSL_SESSION *s);
static void SSL_SESSION_list_add(SSL_CTX *ctx, SSL_SESSION *s);
static int remove_session_lock(SSL_CTX *ctx, SSL_SESSION *c, int lck);

SSL_SESSION *SSL_get_session(const SSL *ssl)
/* aka SSL_get0_session; gets 0 objects, just returns a copy of the pointer */
{
    return (ssl->session);
}

SSL_SESSION *SSL_get1_session(SSL *ssl)
/* variant of SSL_get_session: caller really gets something */
{
    SSL_SESSION *sess;
    /*
     * Need to lock this all up rather than just use CRYPTO_add so that
     * somebody doesn't free ssl->session between when we check it's non-null
     * and when we up the reference count.
     */
    CRYPTO_THREAD_read_lock(ssl->lock);
    sess = ssl->session;
    if (sess)
        SSL_SESSION_up_ref(sess);
    CRYPTO_THREAD_unlock(ssl->lock);
    return sess;
}

int SSL_SESSION_set_ex_data(SSL_SESSION *s, int idx, void *arg)
{
    return (CRYPTO_set_ex_data(&s->ex_data, idx, arg));
}

void *SSL_SESSION_get_ex_data(const SSL_SESSION *s, int idx)
{
    return (CRYPTO_get_ex_data(&s->ex_data, idx));
}

SSL_SESSION *SSL_SESSION_new(void)
{
    SSL_SESSION *ss;

    ss = OPENSSL_zalloc(sizeof(*ss));
    if (ss == NULL) {
        SSLerr(SSL_F_SSL_SESSION_NEW, ERR_R_MALLOC_FAILURE);
        return NULL;
    }

    ss->verify_result = 1;      /* avoid 0 (= X509_V_OK) just in case */
    ss->references = 1;
    ss->timeout = 60 * 5 + 4;   /* 5 minute timeout by default */
    ss->time = (unsigned long)time(NULL);
    ss->lock = CRYPTO_THREAD_lock_new();
    if (ss->lock == NULL) {
        SSLerr(SSL_F_SSL_SESSION_NEW, ERR_R_MALLOC_FAILURE);
        OPENSSL_free(ss);
        return NULL;
    }

    if (!CRYPTO_new_ex_data(CRYPTO_EX_INDEX_SSL_SESSION, ss, &ss->ex_data)) {
        CRYPTO_THREAD_lock_free(ss->lock);
        OPENSSL_free(ss);
        return NULL;
    }
    return ss;
}

/*
 * Create a new SSL_SESSION and duplicate the contents of |src| into it. If
 * ticket == 0 then no ticket information is duplicated, otherwise it is.
 */
SSL_SESSION *ssl_session_dup(SSL_SESSION *src, int ticket)
{
    SSL_SESSION *dest;

    dest = OPENSSL_malloc(sizeof(*src));
    if (dest == NULL) {
        goto err;
    }
    memcpy(dest, src, sizeof(*dest));

    /*
     * Set the various pointers to NULL so that we can call SSL_SESSION_free in
     * the case of an error whilst halfway through constructing dest
     */
#ifndef OPENSSL_NO_PSK
    dest->psk_identity_hint = NULL;
    dest->psk_identity = NULL;
#endif
    dest->ciphers = NULL;
    dest->tlsext_hostname = NULL;
#ifndef OPENSSL_NO_EC
    dest->tlsext_ecpointformatlist = NULL;
    dest->tlsext_ellipticcurvelist = NULL;
#endif
    dest->tlsext_tick = NULL;
#ifndef OPENSSL_NO_SRP
    dest->srp_username = NULL;
#endif
    memset(&dest->ex_data, 0, sizeof(dest->ex_data));

    /* We deliberately don't copy the prev and next pointers */
    dest->prev = NULL;
    dest->next = NULL;

    dest->references = 1;

    dest->lock = CRYPTO_THREAD_lock_new();
    if (dest->lock == NULL)
        goto err;

    if (src->peer != NULL)
        X509_up_ref(src->peer);

    if (src->peer_chain != NULL) {
        dest->peer_chain = X509_chain_up_ref(src->peer_chain);
        if (dest->peer_chain == NULL)
            goto err;
    }
#ifndef OPENSSL_NO_PSK
    if (src->psk_identity_hint) {
        dest->psk_identity_hint = OPENSSL_strdup(src->psk_identity_hint);
        if (dest->psk_identity_hint == NULL) {
            goto err;
        }
    }
    if (src->psk_identity) {
        dest->psk_identity = OPENSSL_strdup(src->psk_identity);
        if (dest->psk_identity == NULL) {
            goto err;
        }
    }
#endif

    if (src->ciphers != NULL) {
        dest->ciphers = sk_SSL_CIPHER_dup(src->ciphers);
        if (dest->ciphers == NULL)
            goto err;
    }

    if (!CRYPTO_dup_ex_data(CRYPTO_EX_INDEX_SSL_SESSION,
                            &dest->ex_data, &src->ex_data)) {
        goto err;
    }

    if (src->tlsext_hostname) {
        dest->tlsext_hostname = OPENSSL_strdup(src->tlsext_hostname);
        if (dest->tlsext_hostname == NULL) {
            goto err;
        }
    }
#ifndef OPENSSL_NO_EC
    if (src->tlsext_ecpointformatlist) {
        dest->tlsext_ecpointformatlist =
            OPENSSL_memdup(src->tlsext_ecpointformatlist,
                           src->tlsext_ecpointformatlist_length);
        if (dest->tlsext_ecpointformatlist == NULL)
            goto err;
    }
    if (src->tlsext_ellipticcurvelist) {
        dest->tlsext_ellipticcurvelist =
            OPENSSL_memdup(src->tlsext_ellipticcurvelist,
                           src->tlsext_ellipticcurvelist_length);
        if (dest->tlsext_ellipticcurvelist == NULL)
            goto err;
    }
#endif

    if (ticket != 0) {
        dest->tlsext_tick =
            OPENSSL_memdup(src->tlsext_tick, src->tlsext_ticklen);
        if (dest->tlsext_tick == NULL)
            goto err;
    } else {
        dest->tlsext_tick_lifetime_hint = 0;
        dest->tlsext_ticklen = 0;
    }

#ifndef OPENSSL_NO_SRP
    if (src->srp_username) {
        dest->srp_username = OPENSSL_strdup(src->srp_username);
        if (dest->srp_username == NULL) {
            goto err;
        }
    }
#endif

    return dest;
 err:
    SSLerr(SSL_F_SSL_SESSION_DUP, ERR_R_MALLOC_FAILURE);
    SSL_SESSION_free(dest);
    return NULL;
}

const unsigned char *SSL_SESSION_get_id(const SSL_SESSION *s, unsigned int *len)
{
    if (len)
        *len = s->session_id_length;
    return s->session_id;
}
const unsigned char *SSL_SESSION_get0_id_context(const SSL_SESSION *s,
                                                unsigned int *len)
{
    if (len != NULL)
        *len = s->sid_ctx_length;
    return s->sid_ctx;
}

unsigned int SSL_SESSION_get_compress_id(const SSL_SESSION *s)
{
    return s->compress_meth;
}

/*
 * SSLv3/TLSv1 has 32 bytes (256 bits) of session ID space. As such, filling
 * the ID with random junk repeatedly until we have no conflict is going to
 * complete in one iteration pretty much "most" of the time (btw:
 * understatement). So, if it takes us 10 iterations and we still can't avoid
 * a conflict - well that's a reasonable point to call it quits. Either the
 * RAND code is broken or someone is trying to open roughly very close to
 * 2^256 SSL sessions to our server. How you might store that many sessions
 * is perhaps a more interesting question ...
 */

#define MAX_SESS_ID_ATTEMPTS 10
static int def_generate_session_id(const SSL *ssl, unsigned char *id,
                                   unsigned int *id_len)
{
    unsigned int retry = 0;
    do
        if (RAND_bytes(id, *id_len) <= 0)
            return 0;
    while (SSL_has_matching_session_id(ssl, id, *id_len) &&
           (++retry < MAX_SESS_ID_ATTEMPTS)) ;
    if (retry < MAX_SESS_ID_ATTEMPTS)
        return 1;
    /* else - woops a session_id match */
    /*
     * XXX We should also check the external cache -- but the probability of
     * a collision is negligible, and we could not prevent the concurrent
     * creation of sessions with identical IDs since we currently don't have
     * means to atomically check whether a session ID already exists and make
     * a reservation for it if it does not (this problem applies to the
     * internal cache as well).
     */
    return 0;
}

int ssl_get_new_session(SSL *s, int session)
{
    /* This gets used by clients and servers. */

    unsigned int tmp;
    SSL_SESSION *ss = NULL;
    GEN_SESSION_CB cb = def_generate_session_id;

    if ((ss = SSL_SESSION_new()) == NULL)
        return (0);

    /* If the context has a default timeout, use it */
    if (s->session_ctx->session_timeout == 0)
        ss->timeout = SSL_get_default_timeout(s);
    else
        ss->timeout = s->session_ctx->session_timeout;

    SSL_SESSION_free(s->session);
    s->session = NULL;

    if (session) {
        if (s->version == SSL3_VERSION) {
            ss->ssl_version = SSL3_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == TLS1_VERSION) {
            ss->ssl_version = TLS1_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == TLS1_1_VERSION) {
            ss->ssl_version = TLS1_1_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == TLS1_2_VERSION) {
            ss->ssl_version = TLS1_2_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == DTLS1_BAD_VER) {
            ss->ssl_version = DTLS1_BAD_VER;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == DTLS1_VERSION) {
            ss->ssl_version = DTLS1_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == DTLS1_2_VERSION) {
            ss->ssl_version = DTLS1_2_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else {
            SSLerr(SSL_F_SSL_GET_NEW_SESSION, SSL_R_UNSUPPORTED_SSL_VERSION);
            SSL_SESSION_free(ss);
            return (0);
        }

        /*-
         * If RFC5077 ticket, use empty session ID (as server).
         * Note that:
         * (a) ssl_get_prev_session() does lookahead into the
         *     ClientHello extensions to find the session ticket.
         *     When ssl_get_prev_session() fails, statem_srvr.c calls
         *     ssl_get_new_session() in tls_process_client_hello().
         *     At that point, it has not yet parsed the extensions,
         *     however, because of the lookahead, it already knows
         *     whether a ticket is expected or not.
         *
         * (b) statem_clnt.c calls ssl_get_new_session() before parsing
         *     ServerHello extensions, and before recording the session
         *     ID received from the server, so this block is a noop.
         */
        if (s->tlsext_ticket_expected) {
            ss->session_id_length = 0;
            goto sess_id_done;
        }

        /* Choose which callback will set the session ID */
        CRYPTO_THREAD_read_lock(s->lock);
        CRYPTO_THREAD_read_lock(s->session_ctx->lock);
        if (s->generate_session_id)
            cb = s->generate_session_id;
        else if (s->session_ctx->generate_session_id)
            cb = s->session_ctx->generate_session_id;
        CRYPTO_THREAD_unlock(s->session_ctx->lock);
        CRYPTO_THREAD_unlock(s->lock);
        /* Choose a session ID */
        memset(ss->session_id, 0, ss->session_id_length);
        tmp = ss->session_id_length;
        if (!cb(s, ss->session_id, &tmp)) {
            /* The callback failed */
            SSLerr(SSL_F_SSL_GET_NEW_SESSION,
                   SSL_R_SSL_SESSION_ID_CALLBACK_FAILED);
            SSL_SESSION_free(ss);
            return (0);
        }
        /*
         * Don't allow the callback to set the session length to zero. nor
         * set it higher than it was.
         */
        if (tmp == 0 || tmp > ss->session_id_length) {
            /* The callback set an illegal length */
            SSLerr(SSL_F_SSL_GET_NEW_SESSION,
                   SSL_R_SSL_SESSION_ID_HAS_BAD_LENGTH);
            SSL_SESSION_free(ss);
            return (0);
        }
        ss->session_id_length = tmp;
        /* Finally, check for a conflict */
        if (SSL_has_matching_session_id(s, ss->session_id,
                                        ss->session_id_length)) {
            SSLerr(SSL_F_SSL_GET_NEW_SESSION, SSL_R_SSL_SESSION_ID_CONFLICT);
            SSL_SESSION_free(ss);
            return (0);
        }

 sess_id_done:
        if (s->tlsext_hostname) {
            ss->tlsext_hostname = OPENSSL_strdup(s->tlsext_hostname);
            if (ss->tlsext_hostname == NULL) {
                SSLerr(SSL_F_SSL_GET_NEW_SESSION, ERR_R_INTERNAL_ERROR);
                SSL_SESSION_free(ss);
                return 0;
            }
        }
    } else {
        ss->session_id_length = 0;
    }

    if (s->sid_ctx_length > sizeof ss->sid_ctx) {
        SSLerr(SSL_F_SSL_GET_NEW_SESSION, ERR_R_INTERNAL_ERROR);
        SSL_SESSION_free(ss);
        return 0;
    }
    memcpy(ss->sid_ctx, s->sid_ctx, s->sid_ctx_length);
    ss->sid_ctx_length = s->sid_ctx_length;
    s->session = ss;
    ss->ssl_version = s->version;
    ss->verify_result = X509_V_OK;

    /* If client supports extended master secret set it in session */
    if (s->s3->flags & TLS1_FLAGS_RECEIVED_EXTMS)
        ss->flags |= SSL_SESS_FLAG_EXTMS;

    return (1);
}

/*-
 * ssl_get_prev attempts to find an SSL_SESSION to be used to resume this
 * connection. It is only called by servers.
 *
 *   ext: ClientHello extensions (including length prefix)
 *   session_id: ClientHello session ID.
 *
 * Returns:
 *   -1: error
 *    0: a session may have been found.
 *
 * Side effects:
 *   - If a session is found then s->session is pointed at it (after freeing an
 *     existing session if need be) and s->verify_result is set from the session.
 *   - Both for new and resumed sessions, s->tlsext_ticket_expected is set to 1
 *     if the server should issue a new session ticket (to 0 otherwise).
 */
int ssl_get_prev_session(SSL *s, const PACKET *ext, const PACKET *session_id)
{
    /* This is used only by servers. */

    SSL_SESSION *ret = NULL;
    int fatal = 0;
    int try_session_cache = 1;
    int r;

    if (PACKET_remaining(session_id) == 0)
        try_session_cache = 0;

    /* sets s->tlsext_ticket_expected and extended master secret flag */
    r = tls_check_serverhello_tlsext_early(s, ext, session_id, &ret);
    switch (r) {
    case -1:                   /* Error during processing */
        fatal = 1;
        goto err;
    case 0:                    /* No ticket found */
    case 1:                    /* Zero length ticket found */
        break;                  /* Ok to carry on processing session id. */
    case 2:                    /* Ticket found but not decrypted. */
    case 3:                    /* Ticket decrypted, *ret has been set. */
        try_session_cache = 0;
        break;
    default:
        abort();
    }

    if (try_session_cache &&
        ret == NULL &&
        !(s->session_ctx->session_cache_mode &
          SSL_SESS_CACHE_NO_INTERNAL_LOOKUP)) {
        SSL_SESSION data;
        size_t local_len;
        data.ssl_version = s->version;
        memset(data.session_id, 0, sizeof(data.session_id));
        if (!PACKET_copy_all(session_id, data.session_id,
                             sizeof(data.session_id), &local_len)) {
            goto err;
        }
        data.session_id_length = local_len;
        CRYPTO_THREAD_read_lock(s->session_ctx->lock);
        ret = lh_SSL_SESSION_retrieve(s->session_ctx->sessions, &data);
        if (ret != NULL) {
            /* don't allow other threads to steal it: */
            SSL_SESSION_up_ref(ret);
        }
        CRYPTO_THREAD_unlock(s->session_ctx->lock);
        if (ret == NULL)
            s->session_ctx->stats.sess_miss++;
    }

    if (try_session_cache &&
        ret == NULL && s->session_ctx->get_session_cb != NULL) {
        int copy = 1;
        ret = s->session_ctx->get_session_cb(s, PACKET_data(session_id),
                                             PACKET_remaining(session_id),
                                             &copy);

        if (ret != NULL) {
            s->session_ctx->stats.sess_cb_hit++;

            /*
             * Increment reference count now if the session callback asks us
             * to do so (note that if the session structures returned by the
             * callback are shared between threads, it must handle the
             * reference count itself [i.e. copy == 0], or things won't be
             * thread-safe).
             */
            if (copy)
                SSL_SESSION_up_ref(ret);

            /*
             * Add the externally cached session to the internal cache as
             * well if and only if we are supposed to.
             */
            if (!
                (s->session_ctx->session_cache_mode &
                 SSL_SESS_CACHE_NO_INTERNAL_STORE)) {
                /*
                 * The following should not return 1, otherwise, things are
                 * very strange
                 */
                if (SSL_CTX_add_session(s->session_ctx, ret))
                    goto err;
            }
        }
    }

    if (ret == NULL)
        goto err;

    /* Now ret is non-NULL and we own one of its reference counts. */

    if (ret->sid_ctx_length != s->sid_ctx_length
        || memcmp(ret->sid_ctx, s->sid_ctx, ret->sid_ctx_length)) {
        /*
         * We have the session requested by the client, but we don't want to
         * use it in this context.
         */
        goto err;               /* treat like cache miss */
    }

    if ((s->verify_mode & SSL_VERIFY_PEER) && s->sid_ctx_length == 0) {
        /*
         * We can't be sure if this session is being used out of context,
         * which is especially important for SSL_VERIFY_PEER. The application
         * should have used SSL[_CTX]_set_session_id_context. For this error
         * case, we generate an error instead of treating the event like a
         * cache miss (otherwise it would be easy for applications to
         * effectively disable the session cache by accident without anyone
         * noticing).
         */

        SSLerr(SSL_F_SSL_GET_PREV_SESSION,
               SSL_R_SESSION_ID_CONTEXT_UNINITIALIZED);
        fatal = 1;
        goto err;
    }

    if (ret->cipher == NULL) {
        unsigned char buf[5], *p;
        unsigned long l;

        p = buf;
        l = ret->cipher_id;
        l2n(l, p);
        if ((ret->ssl_version >> 8) >= SSL3_VERSION_MAJOR)
            ret->cipher = ssl_get_cipher_by_char(s, &(buf[2]));
        else
            ret->cipher = ssl_get_cipher_by_char(s, &(buf[1]));
        if (ret->cipher == NULL)
            goto err;
    }

    if (ret->timeout < (long)(time(NULL) - ret->time)) { /* timeout */
        s->session_ctx->stats.sess_timeout++;
        if (try_session_cache) {
            /* session was from the cache, so remove it */
            SSL_CTX_remove_session(s->session_ctx, ret);
        }
        goto err;
    }

    /* Check extended master secret extension consistency */
    if (ret->flags & SSL_SESS_FLAG_EXTMS) {
        /* If old session includes extms, but new does not: abort handshake */
        if (!(s->s3->flags & TLS1_FLAGS_RECEIVED_EXTMS)) {
            SSLerr(SSL_F_SSL_GET_PREV_SESSION, SSL_R_INCONSISTENT_EXTMS);
            ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_HANDSHAKE_FAILURE);
            fatal = 1;
            goto err;
        }
    } else if (s->s3->flags & TLS1_FLAGS_RECEIVED_EXTMS) {
        /* If new session includes extms, but old does not: do not resume */
        goto err;
    }

    s->session_ctx->stats.sess_hit++;

    SSL_SESSION_free(s->session);
    s->session = ret;
    s->verify_result = s->session->verify_result;
    return 1;

 err:
    if (ret != NULL) {
        SSL_SESSION_free(ret);

        if (!try_session_cache) {
            /*
             * The session was from a ticket, so we should issue a ticket for
             * the new session
             */
            s->tlsext_ticket_expected = 1;
        }
    }
    if (fatal)
        return -1;
    else
        return 0;
}

int SSL_CTX_add_session(SSL_CTX *ctx, SSL_SESSION *c)
{
    int ret = 0;
    SSL_SESSION *s;

    /*
     * add just 1 reference count for the SSL_CTX's session cache even though
     * it has two ways of access: each session is in a doubly linked list and
     * an lhash
     */
    SSL_SESSION_up_ref(c);
    /*
     * if session c is in already in cache, we take back the increment later
     */

    CRYPTO_THREAD_write_lock(ctx->lock);
    s = lh_SSL_SESSION_insert(ctx->sessions, c);

    /*
     * s != NULL iff we already had a session with the given PID. In this
     * case, s == c should hold (then we did not really modify
     * ctx->sessions), or we're in trouble.
     */
    if (s != NULL && s != c) {
        /* We *are* in trouble ... */
        SSL_SESSION_list_remove(ctx, s);
        SSL_SESSION_free(s);
        /*
         * ... so pretend the other session did not exist in cache (we cannot
         * handle two SSL_SESSION structures with identical session ID in the
         * same cache, which could happen e.g. when two threads concurrently
         * obtain the same session from an external cache)
         */
        s = NULL;
    } else if (s == NULL &&
               lh_SSL_SESSION_retrieve(ctx->sessions, c) == NULL) {
        /* s == NULL can also mean OOM error in lh_SSL_SESSION_insert ... */

        /*
         * ... so take back the extra reference and also don't add
         * the session to the SSL_SESSION_list at this time
         */
        s = c;
    }

    /* Put at the head of the queue unless it is already in the cache */
    if (s == NULL)
        SSL_SESSION_list_add(ctx, c);

    if (s != NULL) {
        /*
         * existing cache entry -- decrement previously incremented reference
         * count because it already takes into account the cache
         */

        SSL_SESSION_free(s);    /* s == c */
        ret = 0;
    } else {
        /*
         * new cache entry -- remove old ones if cache has become too large
         */

        ret = 1;

        if (SSL_CTX_sess_get_cache_size(ctx) > 0) {
            while (SSL_CTX_sess_number(ctx) > SSL_CTX_sess_get_cache_size(ctx)) {
                if (!remove_session_lock(ctx, ctx->session_cache_tail, 0))
                    break;
                else
                    ctx->stats.sess_cache_full++;
            }
        }
    }
    CRYPTO_THREAD_unlock(ctx->lock);
    return ret;
}

int SSL_CTX_remove_session(SSL_CTX *ctx, SSL_SESSION *c)
{
    return remove_session_lock(ctx, c, 1);
}

static int remove_session_lock(SSL_CTX *ctx, SSL_SESSION *c, int lck)
{
    SSL_SESSION *r;
    int ret = 0;

    if ((c != NULL) && (c->session_id_length != 0)) {
        if (lck)
            CRYPTO_THREAD_write_lock(ctx->lock);
        if ((r = lh_SSL_SESSION_retrieve(ctx->sessions, c)) == c) {
            ret = 1;
            r = lh_SSL_SESSION_delete(ctx->sessions, c);
            SSL_SESSION_list_remove(ctx, c);
        }
        c->not_resumable = 1;

        if (lck)
            CRYPTO_THREAD_unlock(ctx->lock);

        if (ret)
            SSL_SESSION_free(r);

        if (ctx->remove_session_cb != NULL)
            ctx->remove_session_cb(ctx, c);
    } else
        ret = 0;
    return (ret);
}

void SSL_SESSION_free(SSL_SESSION *ss)
{
    int i;

    if (ss == NULL)
        return;

    CRYPTO_atomic_add(&ss->references, -1, &i, ss->lock);
    REF_PRINT_COUNT("SSL_SESSION", ss);
    if (i > 0)
        return;
    REF_ASSERT_ISNT(i < 0);

    CRYPTO_free_ex_data(CRYPTO_EX_INDEX_SSL_SESSION, ss, &ss->ex_data);

    OPENSSL_cleanse(ss->master_key, sizeof ss->master_key);
    OPENSSL_cleanse(ss->session_id, sizeof ss->session_id);
    X509_free(ss->peer);
    sk_X509_pop_free(ss->peer_chain, X509_free);
    sk_SSL_CIPHER_free(ss->ciphers);
    OPENSSL_free(ss->tlsext_hostname);
    OPENSSL_free(ss->tlsext_tick);
#ifndef OPENSSL_NO_EC
    ss->tlsext_ecpointformatlist_length = 0;
    OPENSSL_free(ss->tlsext_ecpointformatlist);
    ss->tlsext_ellipticcurvelist_length = 0;
    OPENSSL_free(ss->tlsext_ellipticcurvelist);
#endif                          /* OPENSSL_NO_EC */
#ifndef OPENSSL_NO_PSK
    OPENSSL_free(ss->psk_identity_hint);
    OPENSSL_free(ss->psk_identity);
#endif
#ifndef OPENSSL_NO_SRP
    OPENSSL_free(ss->srp_username);
#endif
    CRYPTO_THREAD_lock_free(ss->lock);
    OPENSSL_clear_free(ss, sizeof(*ss));
}

int SSL_SESSION_up_ref(SSL_SESSION *ss)
{
    int i;

    if (CRYPTO_atomic_add(&ss->references, 1, &i, ss->lock) <= 0)
        return 0;

    REF_PRINT_COUNT("SSL_SESSION", ss);
    REF_ASSERT_ISNT(i < 2);
    return ((i > 1) ? 1 : 0);
}

int SSL_set_session(SSL *s, SSL_SESSION *session)
{
    ssl_clear_bad_session(s);
    if (s->ctx->method != s->method) {
        if (!SSL_set_ssl_method(s, s->ctx->method))
            return 0;
    }

    if (session != NULL) {
        SSL_SESSION_up_ref(session);
        s->verify_result = session->verify_result;
    }
    SSL_SESSION_free(s->session);
    s->session = session;

    return 1;
}

int SSL_SESSION_set1_id(SSL_SESSION *s, const unsigned char *sid,
                        unsigned int sid_len)
{
    if (sid_len > SSL_MAX_SSL_SESSION_ID_LENGTH) {
      SSLerr(SSL_F_SSL_SESSION_SET1_ID,
             SSL_R_SSL_SESSION_ID_TOO_LONG);
      return 0;
    }
    s->session_id_length = sid_len;
    memcpy(s->session_id, sid, sid_len);
    return 1;
}

long SSL_SESSION_set_timeout(SSL_SESSION *s, long t)
{
    if (s == NULL)
        return (0);
    s->timeout = t;
    return (1);
}

long SSL_SESSION_get_timeout(const SSL_SESSION *s)
{
    if (s == NULL)
        return (0);
    return (s->timeout);
}

long SSL_SESSION_get_time(const SSL_SESSION *s)
{
    if (s == NULL)
        return (0);
    return (s->time);
}

long SSL_SESSION_set_time(SSL_SESSION *s, long t)
{
    if (s == NULL)
        return (0);
    s->time = t;
    return (t);
}

int SSL_SESSION_get_protocol_version(const SSL_SESSION *s)
{
    return s->ssl_version;
}

const SSL_CIPHER *SSL_SESSION_get0_cipher(const SSL_SESSION *s)
{
    return s->cipher;
}

const char *SSL_SESSION_get0_hostname(const SSL_SESSION *s)
{
    return s->tlsext_hostname;
}

int SSL_SESSION_has_ticket(const SSL_SESSION *s)
{
    return (s->tlsext_ticklen > 0) ? 1 : 0;
}

unsigned long SSL_SESSION_get_ticket_lifetime_hint(const SSL_SESSION *s)
{
    return s->tlsext_tick_lifetime_hint;
}

void SSL_SESSION_get0_ticket(const SSL_SESSION *s, const unsigned char **tick,
                             size_t *len)
{
    *len = s->tlsext_ticklen;
    if (tick != NULL)
        *tick = s->tlsext_tick;
}

X509 *SSL_SESSION_get0_peer(SSL_SESSION *s)
{
    return s->peer;
}

int SSL_SESSION_set1_id_context(SSL_SESSION *s, const unsigned char *sid_ctx,
                                unsigned int sid_ctx_len)
{
    if (sid_ctx_len > SSL_MAX_SID_CTX_LENGTH) {
        SSLerr(SSL_F_SSL_SESSION_SET1_ID_CONTEXT,
               SSL_R_SSL_SESSION_ID_CONTEXT_TOO_LONG);
        return 0;
    }
    s->sid_ctx_length = sid_ctx_len;
    memcpy(s->sid_ctx, sid_ctx, sid_ctx_len);

    return 1;
}

long SSL_CTX_set_timeout(SSL_CTX *s, long t)
{
    long l;
    if (s == NULL)
        return (0);
    l = s->session_timeout;
    s->session_timeout = t;
    return (l);
}

long SSL_CTX_get_timeout(const SSL_CTX *s)
{
    if (s == NULL)
        return (0);
    return (s->session_timeout);
}

int SSL_set_session_secret_cb(SSL *s,
                              int (*tls_session_secret_cb) (SSL *s,
                                                            void *secret,
                                                            int *secret_len,
                                                            STACK_OF(SSL_CIPHER)
                                                            *peer_ciphers,
                                                            const SSL_CIPHER
                                                            **cipher,
                                                            void *arg),
                              void *arg)
{
    if (s == NULL)
        return (0);
    s->tls_session_secret_cb = tls_session_secret_cb;
    s->tls_session_secret_cb_arg = arg;
    return (1);
}

int SSL_set_session_ticket_ext_cb(SSL *s, tls_session_ticket_ext_cb_fn cb,
                                  void *arg)
{
    if (s == NULL)
        return (0);
    s->tls_session_ticket_ext_cb = cb;
    s->tls_session_ticket_ext_cb_arg = arg;
    return (1);
}

int SSL_set_session_ticket_ext(SSL *s, void *ext_data, int ext_len)
{
    if (s->version >= TLS1_VERSION) {
        OPENSSL_free(s->tlsext_session_ticket);
        s->tlsext_session_ticket = NULL;
        s->tlsext_session_ticket =
            OPENSSL_malloc(sizeof(TLS_SESSION_TICKET_EXT) + ext_len);
        if (s->tlsext_session_ticket == NULL) {
            SSLerr(SSL_F_SSL_SET_SESSION_TICKET_EXT, ERR_R_MALLOC_FAILURE);
            return 0;
        }

        if (ext_data) {
            s->tlsext_session_ticket->length = ext_len;
            s->tlsext_session_ticket->data = s->tlsext_session_ticket + 1;
            memcpy(s->tlsext_session_ticket->data, ext_data, ext_len);
        } else {
            s->tlsext_session_ticket->length = 0;
            s->tlsext_session_ticket->data = NULL;
        }

        return 1;
    }

    return 0;
}

typedef struct timeout_param_st {
    SSL_CTX *ctx;
    long time;
    LHASH_OF(SSL_SESSION) *cache;
} TIMEOUT_PARAM;

static void timeout_cb(SSL_SESSION *s, TIMEOUT_PARAM *p)
{
    if ((p->time == 0) || (p->time > (s->time + s->timeout))) { /* timeout */
        /*
         * The reason we don't call SSL_CTX_remove_session() is to save on
         * locking overhead
         */
        (void)lh_SSL_SESSION_delete(p->cache, s);
        SSL_SESSION_list_remove(p->ctx, s);
        s->not_resumable = 1;
        if (p->ctx->remove_session_cb != NULL)
            p->ctx->remove_session_cb(p->ctx, s);
        SSL_SESSION_free(s);
    }
}

IMPLEMENT_LHASH_DOALL_ARG(SSL_SESSION, TIMEOUT_PARAM);

void SSL_CTX_flush_sessions(SSL_CTX *s, long t)
{
    unsigned long i;
    TIMEOUT_PARAM tp;

    tp.ctx = s;
    tp.cache = s->sessions;
    if (tp.cache == NULL)
        return;
    tp.time = t;
    CRYPTO_THREAD_write_lock(s->lock);
    i = lh_SSL_SESSION_get_down_load(s->sessions);
    lh_SSL_SESSION_set_down_load(s->sessions, 0);
    lh_SSL_SESSION_doall_TIMEOUT_PARAM(tp.cache, timeout_cb, &tp);
    lh_SSL_SESSION_set_down_load(s->sessions, i);
    CRYPTO_THREAD_unlock(s->lock);
}

int ssl_clear_bad_session(SSL *s)
{
    if ((s->session != NULL) &&
        !(s->shutdown & SSL_SENT_SHUTDOWN) &&
        !(SSL_in_init(s) || SSL_in_before(s))) {
        SSL_CTX_remove_session(s->session_ctx, s->session);
        return (1);
    } else
        return (0);
}

/* locked by SSL_CTX in the calling function */
static void SSL_SESSION_list_remove(SSL_CTX *ctx, SSL_SESSION *s)
{
    if ((s->next == NULL) || (s->prev == NULL))
        return;

    if (s->next == (SSL_SESSION *)&(ctx->session_cache_tail)) {
        /* last element in list */
        if (s->prev == (SSL_SESSION *)&(ctx->session_cache_head)) {
            /* only one element in list */
            ctx->session_cache_head = NULL;
            ctx->session_cache_tail = NULL;
        } else {
            ctx->session_cache_tail = s->prev;
            s->prev->next = (SSL_SESSION *)&(ctx->session_cache_tail);
        }
    } else {
        if (s->prev == (SSL_SESSION *)&(ctx->session_cache_head)) {
            /* first element in list */
            ctx->session_cache_head = s->next;
            s->next->prev = (SSL_SESSION *)&(ctx->session_cache_head);
        } else {
            /* middle of list */
            s->next->prev = s->prev;
            s->prev->next = s->next;
        }
    }
    s->prev = s->next = NULL;
}

static void SSL_SESSION_list_add(SSL_CTX *ctx, SSL_SESSION *s)
{
    if ((s->next != NULL) && (s->prev != NULL))
        SSL_SESSION_list_remove(ctx, s);

    if (ctx->session_cache_head == NULL) {
        ctx->session_cache_head = s;
        ctx->session_cache_tail = s;
        s->prev = (SSL_SESSION *)&(ctx->session_cache_head);
        s->next = (SSL_SESSION *)&(ctx->session_cache_tail);
    } else {
        s->next = ctx->session_cache_head;
        s->next->prev = s;
        s->prev = (SSL_SESSION *)&(ctx->session_cache_head);
        ctx->session_cache_head = s;
    }
}

void SSL_CTX_sess_set_new_cb(SSL_CTX *ctx,
                             int (*cb) (struct ssl_st *ssl, SSL_SESSION *sess))
{
    ctx->new_session_cb = cb;
}

int (*SSL_CTX_sess_get_new_cb(SSL_CTX *ctx)) (SSL *ssl, SSL_SESSION *sess) {
    return ctx->new_session_cb;
}

void SSL_CTX_sess_set_remove_cb(SSL_CTX *ctx,
                                void (*cb) (SSL_CTX *ctx, SSL_SESSION *sess))
{
    ctx->remove_session_cb = cb;
}

void (*SSL_CTX_sess_get_remove_cb(SSL_CTX *ctx)) (SSL_CTX *ctx,
                                                  SSL_SESSION *sess) {
    return ctx->remove_session_cb;
}

void SSL_CTX_sess_set_get_cb(SSL_CTX *ctx,
                             SSL_SESSION *(*cb) (struct ssl_st *ssl,
                                                 const unsigned char *data,
                                                 int len, int *copy))
{
    ctx->get_session_cb = cb;
}

SSL_SESSION *(*SSL_CTX_sess_get_get_cb(SSL_CTX *ctx)) (SSL *ssl,
                                                       const unsigned char
                                                       *data, int len,
                                                       int *copy) {
    return ctx->get_session_cb;
}

void SSL_CTX_set_info_callback(SSL_CTX *ctx,
                               void (*cb) (const SSL *ssl, int type, int val))
{
    ctx->info_callback = cb;
}

void (*SSL_CTX_get_info_callback(SSL_CTX *ctx)) (const SSL *ssl, int type,
                                                 int val) {
    return ctx->info_callback;
}

void SSL_CTX_set_client_cert_cb(SSL_CTX *ctx,
                                int (*cb) (SSL *ssl, X509 **x509,
                                           EVP_PKEY **pkey))
{
    ctx->client_cert_cb = cb;
}

int (*SSL_CTX_get_client_cert_cb(SSL_CTX *ctx)) (SSL *ssl, X509 **x509,
                                                 EVP_PKEY **pkey) {
    return ctx->client_cert_cb;
}

#ifndef OPENSSL_NO_ENGINE
int SSL_CTX_set_client_cert_engine(SSL_CTX *ctx, ENGINE *e)
{
    if (!ENGINE_init(e)) {
        SSLerr(SSL_F_SSL_CTX_SET_CLIENT_CERT_ENGINE, ERR_R_ENGINE_LIB);
        return 0;
    }
    if (!ENGINE_get_ssl_client_cert_function(e)) {
        SSLerr(SSL_F_SSL_CTX_SET_CLIENT_CERT_ENGINE,
               SSL_R_NO_CLIENT_CERT_METHOD);
        ENGINE_finish(e);
        return 0;
    }
    ctx->client_cert_engine = e;
    return 1;
}
#endif

void SSL_CTX_set_cookie_generate_cb(SSL_CTX *ctx,
                                    int (*cb) (SSL *ssl,
                                               unsigned char *cookie,
                                               unsigned int *cookie_len))
{
    ctx->app_gen_cookie_cb = cb;
}

void SSL_CTX_set_cookie_verify_cb(SSL_CTX *ctx,
                                  int (*cb) (SSL *ssl,
                                             const unsigned char *cookie,
                                             unsigned int cookie_len))
{
    ctx->app_verify_cookie_cb = cb;
}

IMPLEMENT_PEM_rw(SSL_SESSION, SSL_SESSION, PEM_STRING_SSL_SESSION, SSL_SESSION)
