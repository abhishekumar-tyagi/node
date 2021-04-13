#include "quic/crypto.h"

#include "quic/endpoint.h"
#include "quic/session.h"
#include "quic/stream.h"
#include "quic/quic.h"
#include "crypto/crypto_util.h"
#include "crypto/crypto_context.h"
#include "crypto/crypto_common.h"
#include "aliased_struct-inl.h"
#include "async_wrap-inl.h"
#include "base_object-inl.h"
#include "env-inl.h"
#include "node_crypto.h"
#include "node_process-inl.h"
#include "node_sockaddr-inl.h"
#include "node_url.h"
#include "string_bytes.h"
#include "util-inl.h"

#include "v8.h"

#include <ngtcp2/ngtcp2.h>
#include <ngtcp2/ngtcp2_crypto.h>
#include <nghttp3/nghttp3.h>  // NGHTTP3_ALPN_H3
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>

#include <iterator>
#include <limits>
#include <numeric>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>

namespace node {

using crypto::EntropySource;

using v8::ArrayBuffer;
using v8::BackingStore;
using v8::EscapableHandleScope;
using v8::Just;
using v8::Local;
using v8::Maybe;
using v8::MaybeLocal;
using v8::Nothing;
using v8::Value;

namespace quic {

AeadContextPointer::AeadContextPointer(
    Mode mode,
    const uint8_t* key,
    const ngtcp2_crypto_aead& aead) {
  switch (mode) {
    case Mode::ENCRYPT:
      inited_ = NGTCP2_OK(ngtcp2_crypto_aead_ctx_encrypt_init(
          &ctx_,
          &aead,
          key,
          kCryptoTokenIvlen));
      break;
    case Mode::DECRYPT:
      inited_ = NGTCP2_OK(ngtcp2_crypto_aead_ctx_decrypt_init(
          &ctx_,
          &aead,
          key,
          kCryptoTokenIvlen));
      break;
    default:
      UNREACHABLE();
  }
}

bool SessionTicketAppData::Set(const uint8_t* data, size_t len) {
  if (set_) return false;
  set_ = true;
  SSL_SESSION_set1_ticket_appdata(session_, data, len);
  return set_;
}

bool SessionTicketAppData::Get(uint8_t** data, size_t* len) const {
  return SSL_SESSION_get0_ticket_appdata(
      session_,
      reinterpret_cast<void**>(data),
      len) == 1;
}

namespace {
bool DeriveTokenKey(
    uint8_t* token_key,
    uint8_t* token_iv,
    const uint8_t* rand_data,
    size_t rand_datalen,
    const ngtcp2_crypto_aead& aead,
    const ngtcp2_crypto_md& md,
    const uint8_t* token_secret) {
  static constexpr int kCryptoTokenSecretlen = 32;
  uint8_t secret[kCryptoTokenSecretlen];

  return
      NGTCP2_OK(ngtcp2_crypto_hkdf_extract(
          secret,
          &md,
          token_secret,
          kTokenSecretLen,
          rand_data,
          rand_datalen)) &&
      NGTCP2_OK(ngtcp2_crypto_derive_packet_protection_key(
          token_key,
          token_iv,
          nullptr,
          &aead,
          &md,
          secret,
          kCryptoTokenSecretlen));
}

// Retry tokens are generated only by QUIC servers. They
// are opaque to QUIC clients and must not be guessable by
// on- or off-path attackers. A QUIC server sends a RETRY
// token as a way of initiating explicit path validation
// with a client in response to an initial QUIC packet.
// The client, upon receiving a RETRY, must abandon the
// initial connection attempt and try again, including the
// received retry token in the new initial packet sent to
// the server. If the server is performing explicit
// valiation, it will look for the presence of the retry
// token and validate it if found. The internal structure
// of the retry token must be meaningful to the server,
// and the server must be able to validate the token without
// relying on any state left over from the previous connection
// attempt. The implementation here is entirely Node.js
// specific.
//
// The token secret must be kept secret on the QUIC server that
// generated the retry. When multiple QUIC servers are used in a
// cluster, it cannot be guaranteed that the same QUIC server
// instance will receive the subsequent new Initial packet. Therefore,
// all QUIC servers in the cluster should either share or be aware
// of the same token secret or a mechanism needs to be implemented
// to ensure that subsequent packets are routed to the same QUIC
// server instance.
//
// A malicious peer could attempt to guess the token secret by
// sending a large number specially crafted RETRY-eliciting packets
// to a server then analyzing the resulting retry tokens. To reduce
// the possibility of such attacks, the current implementation of
// QuicSocket generates the token secret randomly for each instance,
// and the number of RETRY responses sent to a given remote address
// should be limited. Such attacks should be of little actual value
// in most cases.
bool GenerateRetryToken(
    uint8_t* token,
    size_t* tokenlen,
    const std::shared_ptr<SocketAddress>& addr,
    const CID& retry_cid,
    const CID& ocid,
    const uint8_t* token_secret,
    const ngtcp2_crypto_aead& aead,
    const ngtcp2_crypto_md& md) {
  uint8_t plaintext[32];
  uint8_t aad[256];
  uint8_t rand_data[kTokenRandLen];
  uint8_t token_key[kCryptoTokenKeylen];
  uint8_t token_iv[kCryptoTokenIvlen];

  EntropySource(rand_data, kTokenRandLen);

  if (UNLIKELY(!DeriveTokenKey(
          token_key,
          token_iv,
          rand_data,
          kTokenRandLen,
          aead,
          md,
          token_secret))) {
    return false;
  }

  AeadContextPointer aead_ctx(
      AeadContextPointer::Mode::ENCRYPT,
      token_key,
      aead);
  if (UNLIKELY(!aead_ctx)) return false;

  uint64_t now = uv_hrtime();
  uint8_t* ptr = plaintext;

  // Prepare the plaintext (host order timestamp + ocid)
  size_t plaintextlen = sizeof(uint64_t) + ocid->datalen;
  CHECK_LE(plaintextlen, arraysize(plaintext));
  memcpy(ptr, &now, sizeof(uint64_t));
  memcpy(ptr + sizeof(uint64_t), ocid->data, ocid->datalen);

  ptr = aad;
  size_t aadlen = addr->length() + retry_cid.length();
  CHECK_LE(aadlen, arraysize(aad));
  memcpy(ptr, addr->raw(), addr->length());
  memcpy(ptr + addr->length(), retry_cid.data(), retry_cid.length());

  token[0] = kRetryTokenMagic;

  if (UNLIKELY(NGTCP2_ERR(ngtcp2_crypto_encrypt(
          token + 1,
          &aead,
          aead_ctx.get(),
          plaintext,
          plaintextlen,
          token_iv,
          kCryptoTokenIvlen,
          aad,
          aadlen)))) {
    return false;
  }

  *tokenlen = 1 + plaintextlen + aead.max_overhead;
  memcpy(token + (*tokenlen), rand_data, kTokenRandLen);
  *tokenlen += kTokenRandLen;
  return true;
}
}  // namespace

// A stateless reset token is used when a QUIC endpoint receives a
// QUIC packet with a short header but the associated connection ID
// cannot be matched to any known Session. In such cases, the
// receiver may choose to send a subtle opaque indication to the
// sending peer that state for the Session has apparently been
// lost. For any on- or off- path attacker, a stateless reset packet
// resembles any other QUIC packet with a short header. In order to
// be successfully handled as a stateless reset, the peer must have
// already seen a reset token issued to it associated with the given
// CID. The token itself is opaque to the peer that receives is but
// must be possible to statelessly recreate by the peer that
// originally created it. The actual implementation is Node.js
// specific but we currently defer to a utility function provided
// by ngtcp2.
bool GenerateResetToken(uint8_t* token, const uint8_t* secret, const CID& cid) {
  ngtcp2_crypto_ctx ctx;
  ngtcp2_crypto_ctx_initial(&ctx);
  return NGTCP2_OK(ngtcp2_crypto_generate_stateless_reset_token(
      token,
      &ctx.md,
      secret,
      NGTCP2_STATELESS_RESET_TOKENLEN,
      cid.cid()));
}

// Validates a retry token included in the given header. This will return
// true if the token cannot be validated, false otherwise. A token is
// valid if it can be successfully decrypted using the key derived from
// random data embedded in the token, the structure of the token matches
// that generated by the GenerateRetryToken function, and the token was
// not generated earlier than now - verification_expiration. If validation
// is successful, ocid will be updated to the original connection ID encoded
// in the encrypted token.
Maybe<CID> ValidateRetryToken(
    const ngtcp2_vec& token,
    const std::shared_ptr<SocketAddress>& addr,
    const CID& dcid,
    const uint8_t* token_secret,
    uint64_t verification_expiration,
    const ngtcp2_crypto_aead& aead,
    const ngtcp2_crypto_md& md) {
  uint8_t token_key[kCryptoTokenKeylen];
  uint8_t token_iv[kCryptoTokenIvlen];
  uint8_t plaintext[4096];
  uint8_t aad[256];

  // Quick checks. If the token is too short, too long, or does not
  // contain the right token magic byte, assume invalid and skip further
  // checks.
  if (UNLIKELY(token.len < kMinRetryTokenLen ||
      token.len > kMaxRetryTokenLen ||
      token.base[0] != kRetryTokenMagic)) {
    return Nothing<CID>();
  }

  const uint8_t* ciphertext = token.base + 1;
  const uint8_t* rand_data = token.base + token.len - kTokenRandLen;
  size_t ciphertextlen = token.len - kTokenRandLen - 1;

  if (UNLIKELY(!DeriveTokenKey(
          token_key,
          token_iv,
          rand_data,
          kTokenRandLen,
          aead,
          md,
          token_secret))) {
    return Nothing<CID>();
  }

  AeadContextPointer aead_ctx(
      AeadContextPointer::Mode::DECRYPT,
      token_key,
      aead);
  if (UNLIKELY(!aead_ctx)) return Nothing<CID>();

  // Prepare the additional data (raw socket address + retry_cid)
  uint8_t* ptr = aad;
  size_t aadlen = addr->length() + dcid.length();
  CHECK_LE(aadlen, arraysize(aad));
  memcpy(ptr, addr->raw(), addr->length());
  memcpy(ptr + addr->length(), dcid.data(), dcid.length());

  if (UNLIKELY(NGTCP2_ERR(ngtcp2_crypto_decrypt(
          plaintext,
          &aead,
          aead_ctx.get(),
          ciphertext,
          ciphertextlen,
          token_iv,
          kCryptoTokenIvlen,
          aad,
          aadlen)))) {
    return Nothing<CID>();
  }

  size_t plaintextlen = ciphertextlen - aead.max_overhead;
  if (plaintextlen < sizeof(uint64_t))
    return Nothing<CID>();


  size_t cil = plaintextlen - sizeof(uint64_t);
  if (cil != 0 && (cil < NGTCP2_MIN_CIDLEN || cil > NGTCP2_MAX_CIDLEN))
    return Nothing<CID>();


  uint64_t t;
  memcpy(&t, plaintext, sizeof(uint64_t));

  // 10-second window by default, but configurable for each
  // Endpoint instance with a MIN_RETRYTOKEN_EXPIRATION second
  // minimum and a MAX_RETRYTOKEN_EXPIRATION second maximum.
  if (t + verification_expiration * NGTCP2_SECONDS < uv_hrtime())
    return Nothing<CID>();


  return Just(CID(plaintext + sizeof(uint64_t), cil));
}

// Generates a RETRY packet. See the notes for GenerateRetryToken for details.
std::unique_ptr<Packet> GenerateRetryPacket(
    quic_version version,
    const uint8_t* token_secret,
    const CID& dcid,
    const CID& scid,
    const std::shared_ptr<SocketAddress>& local_addr,
    const std::shared_ptr<SocketAddress>& remote_addr,
    const ngtcp2_crypto_aead& aead,
    const ngtcp2_crypto_md& md) {

  uint8_t token[256];
  size_t tokenlen = sizeof(token);

  CID cid;
  EntropySource(cid.data(), NGTCP2_MAX_CIDLEN);
  cid.set_length(NGTCP2_MAX_CIDLEN);
  if (UNLIKELY(!GenerateRetryToken(
          token,
          &tokenlen,
          remote_addr,
          cid,
          dcid,
          token_secret,
          aead,
          md))) {
    return {};
  }

  size_t pktlen = tokenlen + (2 * NGTCP2_MAX_CIDLEN) + scid.length() + 8;

  std::unique_ptr<Packet> packet = std::make_unique<Packet>(pktlen, "retry");
  ssize_t nwrite =
      ngtcp2_crypto_write_retry(
          packet->data(),
          NGTCP2_MAX_PKTLEN_IPV4,
          version,
          scid.cid(),
          cid.cid(),
          dcid.cid(),
          token,
          tokenlen);
  if (nwrite <= 0)
    return {};
  packet->set_length(nwrite);
  return packet;
}

bool GenerateToken(
    uint8_t* token,
    size_t* tokenlen,
    const std::shared_ptr<SocketAddress>& addr,
    const uint8_t* token_secret,
    const ngtcp2_crypto_aead& aead,
    const ngtcp2_crypto_md& md) {
  uint8_t rand_data[kTokenRandLen];
  uint8_t token_key[kCryptoTokenKeylen];
  uint8_t token_iv[kCryptoTokenIvlen];

  EntropySource(rand_data, kTokenRandLen);

  if (UNLIKELY(!DeriveTokenKey(
          token_key,
          token_iv,
          rand_data,
          kTokenRandLen,
          aead,
          md,
          token_secret))) {
    return false;
  }

  AeadContextPointer aead_ctx(
      AeadContextPointer::Mode::ENCRYPT,
      token_key,
      aead);
  if (UNLIKELY(!aead_ctx)) return false;

  uint64_t now = uv_hrtime();

  token[0] = kTokenMagic;

  if (UNLIKELY(NGTCP2_ERR(ngtcp2_crypto_encrypt(
          token + 1,
          &aead,
          aead_ctx.get(),
          reinterpret_cast<uint8_t*>(&now),
          sizeof(uint64_t),
          token_iv,
          kCryptoTokenIvlen,
          addr->raw(),
          addr->length())))) {
    return false;
  }

  *tokenlen = 1 + sizeof(uint64_t) + aead.max_overhead;
  memcpy(token + (*tokenlen), rand_data, kTokenRandLen);
  *tokenlen += kTokenRandLen;
  return true;
}

bool ValidateToken(
    const ngtcp2_vec& token,
    const std::shared_ptr<SocketAddress>& addr,
    const uint8_t* token_secret,
    uint64_t verification_expiration,
    const ngtcp2_crypto_aead& aead,
    const ngtcp2_crypto_md& md) {
  uint8_t token_key[kCryptoTokenKeylen];
  uint8_t token_iv[kCryptoTokenIvlen];
  uint8_t plaintext[kMaxTokenLen];

  // Quick checks. If the token is too short, too long, or does not
  // contain the right token magic byte, assume invalid and skip further
  // checks.
  if (UNLIKELY(token.len < kMinRetryTokenLen ||
      token.len > kMaxTokenLen ||
      token.base[0] != kTokenMagic)) {
    return false;
  }

  AeadContextPointer aead_ctx(
      AeadContextPointer::Mode::DECRYPT,
      token_key,
      aead);
  if (UNLIKELY(!aead_ctx)) return false;

  const uint8_t* rand_data = token.base + token.len - kTokenRandLen;
  const uint8_t* ciphertext = token.base + 1;
  size_t ciphertextlen = token.len - kTokenRandLen - 1;

  if (UNLIKELY(!DeriveTokenKey(
          token_key,
          token_iv,
          rand_data,
          kTokenRandLen,
          aead,
          md,
          token_secret))) {
    return false;
  }

  if (UNLIKELY(NGTCP2_ERR(ngtcp2_crypto_decrypt(
          plaintext,
          &aead,
          aead_ctx.get(),
          ciphertext,
          ciphertextlen,
          token_iv,
          kCryptoTokenIvlen,
          addr->raw(),
          addr->length())))) {
    return false;
  }

  size_t plaintextlen = ciphertextlen - aead.max_overhead;
  if (plaintextlen != sizeof(uint64_t))
    return false;

  uint64_t t;
  memcpy(&t, plaintext, sizeof(uint64_t));

  // 1 hour window by default, but configurable for each
  // Endpoint instance with a MIN_RETRYTOKEN_EXPIRATION second
  // minimum and a MAX_RETRYTOKEN_EXPIRATION second maximum.
  return t + verification_expiration * NGTCP2_SECONDS >= uv_hrtime();
}

// Get the ALPN protocol identifier that was negotiated for the session
Local<Value> GetALPNProtocol(const Session& session) {
  Session::CryptoContext* ctx = session.crypto_context();
  Environment* env = session.env();
  BindingState* state = BindingState::Get(env);
  std::string alpn = ctx->selected_alpn();
  if (alpn == &NGHTTP3_ALPN_H3[1]) {
    return state->http3_alpn_string();
  } else {
    return ToV8Value(
      env->context(),
      alpn,
      env->isolate()).FromMaybe(Local<Value>());
  }
}

namespace {
int CertCB(SSL* ssl, void* arg) {
  Session* session = static_cast<Session*>(arg);
  int ret;
  switch (SSL_get_tlsext_status_type(ssl)) {
    case TLSEXT_STATUSTYPE_ocsp:
      ret = session->crypto_context()->OnOCSP();
      return UNLIKELY(session->is_destroyed()) ? 0 : ret;
    default:
      return 1;
  }
}

void Keylog_CB(const SSL* ssl, const char* line) {
  Session* session = static_cast<Session*>(SSL_get_app_data(ssl));
  session->crypto_context()->Keylog(line);
}

int Client_Hello_CB(
    SSL* ssl,
    int* tls_alert,
    void* arg) {
  Session* session = static_cast<Session*>(SSL_get_app_data(ssl));

  int ret = session->crypto_context()->OnClientHello();

  if (UNLIKELY(session->is_destroyed())) {
    *tls_alert = SSL_R_SSL_HANDSHAKE_FAILURE;
    return 0;
  }
  switch (ret) {
    case 0:
      return 1;
    case -1:
      return -1;
    default:
      *tls_alert = ret;
      return 0;
  }
}

int AlpnSelection(
    SSL* ssl,
    const unsigned char** out,
    unsigned char* outlen,
    const unsigned char* in,
    unsigned int inlen,
    void* arg) {
  Session* session = static_cast<Session*>(SSL_get_app_data(ssl));

  size_t alpn_len = session->alpn().length();
  if (alpn_len > 255) return SSL_TLSEXT_ERR_NOACK;

  // The QuicServerSession supports exactly one ALPN identifier. If that does
  // not match any of the ALPN identifiers provided in the client request,
  // then we fail here. Note that this will not fail the TLS handshake, so
  // we have to check later if the ALPN matches the expected identifier or not.
  if (SSL_select_next_proto(
          const_cast<unsigned char**>(out),
          outlen,
          reinterpret_cast<const unsigned char*>(session->alpn().c_str()),
          alpn_len,
          in,
          inlen) == OPENSSL_NPN_NO_OVERLAP) {
    return SSL_TLSEXT_ERR_NOACK;
  }

  return SSL_TLSEXT_ERR_OK;
}

int AllowEarlyDataCB(SSL* ssl, void* arg) {
  Session* session = static_cast<Session*>(SSL_get_app_data(ssl));
  return session->allow_early_data() ? 1 : 0;
}

int TLS_Status_Callback(SSL* ssl, void* arg) {
  Session* session = static_cast<Session*>(SSL_get_app_data(ssl));
  return session->crypto_context()->OnTLSStatus();
}

int New_Session_Callback(SSL* ssl, SSL_SESSION* session) {
  Session* s = static_cast<Session*>(SSL_get_app_data(ssl));
  return s->set_session(session);
}

int GenerateSessionTicket(SSL* ssl, void* arg) {
  Session* s = static_cast<Session*>(SSL_get_app_data(ssl));
  SessionTicketAppData app_data(SSL_get_session(ssl));
  s->SetSessionTicketAppData(app_data);
  return 1;
}

SSL_TICKET_RETURN DecryptSessionTicket(
    SSL* ssl,
    SSL_SESSION* session,
    const unsigned char* keyname,
    size_t keyname_len,
    SSL_TICKET_STATUS status,
    void* arg) {
  Session* s = static_cast<Session*>(SSL_get_app_data(ssl));
  SessionTicketAppData::Flag flag = SessionTicketAppData::Flag::STATUS_NONE;
  switch (status) {
    default:
      return SSL_TICKET_RETURN_IGNORE;
    case SSL_TICKET_EMPTY:
      // Fall through
    case SSL_TICKET_NO_DECRYPT:
      return SSL_TICKET_RETURN_IGNORE_RENEW;
    case SSL_TICKET_SUCCESS_RENEW:
      flag = SessionTicketAppData::Flag::STATUS_RENEW;
      // Fall through
    case SSL_TICKET_SUCCESS:
      SessionTicketAppData app_data(session);
      switch (s->GetSessionTicketAppData(app_data, flag)) {
        default:
          return SSL_TICKET_RETURN_IGNORE;
        case SessionTicketAppData::Status::TICKET_IGNORE:
          return SSL_TICKET_RETURN_IGNORE;
        case SessionTicketAppData::Status::TICKET_IGNORE_RENEW:
          return SSL_TICKET_RETURN_IGNORE_RENEW;
        case SessionTicketAppData::Status::TICKET_USE:
          return SSL_TICKET_RETURN_USE;
        case SessionTicketAppData::Status::TICKET_USE_RENEW:
          return SSL_TICKET_RETURN_USE_RENEW;
      }
  }
}

int SetEncryptionSecrets(
    SSL* ssl,
    OSSL_ENCRYPTION_LEVEL ossl_level,
    const uint8_t* read_secret,
    const uint8_t* write_secret,
    size_t secret_len) {
  Session* session = static_cast<Session*>(SSL_get_app_data(ssl));
  return session->crypto_context()->OnSecrets(
      from_ossl_level(ossl_level),
      read_secret,
      write_secret,
      secret_len) ? 1 : 0;
}

int AddHandshakeData(
    SSL* ssl,
    OSSL_ENCRYPTION_LEVEL ossl_level,
    const uint8_t* data,
    size_t len) {
  Session* session = static_cast<Session*>(SSL_get_app_data(ssl));
  session->crypto_context()->WriteHandshake(
      from_ossl_level(ossl_level),
      data,
      len);
  return 1;
}

int FlushFlight(SSL* ssl) { return 1; }

int SendAlert(
    SSL* ssl,
    ssl_encryption_level_t level,
    uint8_t alert) {
  Session* session = static_cast<Session*>(SSL_get_app_data(ssl));
  session->crypto_context()->set_tls_alert(alert);
  return 1;
}

bool SetTransportParams(Session* session, const crypto::SSLPointer& ssl) {
  ngtcp2_transport_params params;
  ngtcp2_conn_get_local_transport_params(session->connection(), &params);
  uint8_t buf[512];
  ssize_t nwrite = ngtcp2_encode_transport_params(
      buf,
      arraysize(buf),
      NGTCP2_TRANSPORT_PARAMS_TYPE_ENCRYPTED_EXTENSIONS,
      &params);
  return nwrite >= 0 &&
         SSL_set_quic_transport_params(ssl.get(), buf, nwrite) == 1;
}

SSL_QUIC_METHOD quic_method = SSL_QUIC_METHOD{
  SetEncryptionSecrets,
  AddHandshakeData,
  FlushFlight,
  SendAlert
};

void SetHostname(const crypto::SSLPointer& ssl, const std::string& hostname) {
  // If the hostname is an IP address, use an empty string
  // as the hostname instead.
    X509_VERIFY_PARAM* param = SSL_get0_param(ssl.get());
    X509_VERIFY_PARAM_set_hostflags(param, 0);

  if (UNLIKELY(SocketAddress::is_numeric_host(hostname.c_str()))) {
    SSL_set_tlsext_host_name(ssl.get(), "");
    CHECK_EQ(X509_VERIFY_PARAM_set1_host(param, "", 0), 1);
    return;
  }

  SSL_set_tlsext_host_name(ssl.get(), hostname.c_str());
  CHECK_EQ(
    X509_VERIFY_PARAM_set1_host(param, hostname.c_str(), hostname.length()),
    1);
}

}  // namespace

void InitializeTLS(Session* session, const crypto::SSLPointer& ssl) {
  Session::CryptoContext* ctx = session->crypto_context();
  Environment* env = session->env();
  BindingState* state = env->GetBindingData<BindingState>(env->context());
  Debug(session, "Initializing TLS for session");
  SSL_set_app_data(ssl.get(), session);
  SSL_set_cert_cb(
      ssl.get(),
      CertCB,
      const_cast<void*>(reinterpret_cast<const void*>(session)));
  SSL_set_verify(
      ssl.get(),
      SSL_VERIFY_NONE,
      crypto::VerifyCallback);

  // Enable tracing if the `--trace-tls` command line flag is used.
  if (UNLIKELY(env->options()->trace_tls || ctx->enable_tls_trace())) {
    ctx->EnableTrace();
    if (state->warn_trace_tls) {
      state->warn_trace_tls = false;
      ProcessEmitWarning(env,
          "Enabling --trace-tls can expose sensitive data "
          "in the resulting log");
    }
  }

  switch (ctx->side()) {
    case NGTCP2_CRYPTO_SIDE_CLIENT: {
      SSL_set_connect_state(ssl.get());
      crypto::SetALPN(ssl, session->alpn());
      SetHostname(ssl, session->hostname());
      if (ctx->request_ocsp())
        SSL_set_tlsext_status_type(ssl.get(), TLSEXT_STATUSTYPE_ocsp);
      break;
    }
    case NGTCP2_CRYPTO_SIDE_SERVER: {
      SSL_set_accept_state(ssl.get());
      if (ctx->request_peer_certificate()) {
        int verify_mode = SSL_VERIFY_PEER;
        if (ctx->reject_unauthorized())
          verify_mode |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
        SSL_set_verify(ssl.get(), verify_mode, crypto::VerifyCallback);
      }
      break;
    }
    default:
      UNREACHABLE();
  }

  ngtcp2_conn_set_tls_native_handle(session->connection(), ssl.get());
  SetTransportParams(session, ssl);
}

void InitializeSecureContext(
    crypto::SecureContext* sc,
    ngtcp2_crypto_side side) {
  sc->ctx_.reset(SSL_CTX_new(TLS_method()));
  SSL_CTX_set_app_data(**sc, sc);

  switch (side) {
    case NGTCP2_CRYPTO_SIDE_SERVER:
      SSL_CTX_set_options(
          **sc,
          (SSL_OP_ALL & ~SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS) |
           SSL_OP_SINGLE_ECDH_USE |
           SSL_OP_CIPHER_SERVER_PREFERENCE);
      SSL_CTX_set_mode(**sc, SSL_MODE_RELEASE_BUFFERS);
      SSL_CTX_set_alpn_select_cb(**sc, AlpnSelection, nullptr);
      SSL_CTX_set_client_hello_cb(**sc, Client_Hello_CB, nullptr);
      SSL_CTX_set_session_ticket_cb(
          **sc,
          GenerateSessionTicket,
          DecryptSessionTicket,
          nullptr);
      SSL_CTX_set_max_early_data(**sc, std::numeric_limits<uint32_t>::max());
      SSL_CTX_set_allow_early_data_cb(**sc, AllowEarlyDataCB, nullptr);
      break;
    case NGTCP2_CRYPTO_SIDE_CLIENT:
      SSL_CTX_set_session_cache_mode(
          **sc,
          SSL_SESS_CACHE_CLIENT |
          SSL_SESS_CACHE_NO_INTERNAL_STORE);
      SSL_CTX_sess_set_new_cb(**sc, New_Session_Callback);
      break;
    default:
      UNREACHABLE();
  }
  SSL_CTX_set_min_proto_version(**sc, TLS1_3_VERSION);
  SSL_CTX_set_max_proto_version(**sc, TLS1_3_VERSION);
  SSL_CTX_set_default_verify_paths(**sc);
  SSL_CTX_set_tlsext_status_cb(**sc, TLS_Status_Callback);
  SSL_CTX_set_keylog_callback(**sc, Keylog_CB);
  SSL_CTX_set_tlsext_status_arg(**sc, nullptr);
  SSL_CTX_set_quic_method(**sc, &quic_method);
}

ngtcp2_crypto_level from_ossl_level(OSSL_ENCRYPTION_LEVEL ossl_level) {
  switch (ossl_level) {
  case ssl_encryption_initial:
    return NGTCP2_CRYPTO_LEVEL_INITIAL;
  case ssl_encryption_early_data:
    return NGTCP2_CRYPTO_LEVEL_EARLY;
  case ssl_encryption_handshake:
    return NGTCP2_CRYPTO_LEVEL_HANDSHAKE;
  case ssl_encryption_application:
    return NGTCP2_CRYPTO_LEVEL_APPLICATION;
  default:
    UNREACHABLE();
  }
}

const char* crypto_level_name(ngtcp2_crypto_level level) {
  switch (level) {
    case NGTCP2_CRYPTO_LEVEL_INITIAL:
      return "initial";
    case NGTCP2_CRYPTO_LEVEL_EARLY:
      return "early";
    case NGTCP2_CRYPTO_LEVEL_HANDSHAKE:
      return "handshake";
    case NGTCP2_CRYPTO_LEVEL_APPLICATION:
      return "app";
    default:
      UNREACHABLE();
  }
}

// When using IPv6, QUIC recommends the use of IPv6 Flow Labels
// as specified in https://tools.ietf.org/html/rfc6437. These
// are used as a means of reliably associating packets exchanged
// as part of a single flow and protecting against certain kinds
// of attacks.
uint32_t GenerateFlowLabel(
    const std::shared_ptr<SocketAddress>& local,
    const std::shared_ptr<SocketAddress>& remote,
    const CID& cid,
    const uint8_t* secret,
    size_t secretlen) {
  static constexpr size_t kInfoLen =
      (sizeof(sockaddr_in6) * 2) + NGTCP2_MAX_CIDLEN;

  uint32_t label = 0;

  uint8_t plaintext[kInfoLen];
  size_t infolen = local->length() + remote->length() + cid.length();
  CHECK_LE(infolen, kInfoLen);

  ngtcp2_crypto_ctx ctx;
  ngtcp2_crypto_ctx_initial(&ctx);

  uint8_t* ptr = plaintext;
  CHECK_LE(local->length() + remote->length() + cid->datalen, kInfoLen);
  memcpy(ptr, local->raw(), local->length());
  ptr += local->length();
  memcpy(ptr, remote->raw(), remote->length());
  ptr += remote->length();
  memcpy(ptr, cid->data, cid->datalen);

  ngtcp2_crypto_hkdf_expand(
      reinterpret_cast<uint8_t*>(&label),
      sizeof(label),
      &ctx.md,
      secret,
      secretlen,
      plaintext,
      infolen);

  label &= kLabelMask;
  DCHECK_LE(label, kLabelMask);
  return label;
}

ngtcp2_crypto_aead CryptoAeadAes128GCM() {
  ngtcp2_crypto_aead aead;
  ngtcp2_crypto_aead_init(&aead, const_cast<EVP_CIPHER *>(EVP_aes_128_gcm()));
  return aead;
}

ngtcp2_crypto_md CryptoMDSha256() {
  ngtcp2_crypto_md md;
  ngtcp2_crypto_md_init(&md, const_cast<EVP_MD *>(EVP_sha256()));
  return md;
}

MaybeLocal<Value> GetCertificateData(
    Environment* env,
    crypto::SecureContext* sc,
    GetCertificateType type) {
  EscapableHandleScope scope(env->isolate());
  X509* cert;
  switch (type) {
    case GetCertificateType::SELF:
      cert = sc->cert_.get();
      break;
    case GetCertificateType::ISSUER:
      cert = sc->issuer_.get();
      break;
    default:
      UNREACHABLE();
  }

  Local<Value> ret = v8::Undefined(env->isolate());
  int size = i2d_X509(cert, nullptr);
  if (size > 0) {
    std::shared_ptr<BackingStore> store =
        ArrayBuffer::NewBackingStore(env->isolate(), size);
    unsigned char* buf = reinterpret_cast<unsigned char*>(store->Data());
    i2d_X509(cert, &buf);
    ret = ArrayBuffer::New(env->isolate(), store);
  }

  return scope.Escape(ret);
}

}  // namespace quic
}  // namespace node
