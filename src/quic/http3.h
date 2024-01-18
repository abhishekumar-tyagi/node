#pragma once

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS
#if HAVE_OPENSSL && NODE_OPENSSL_HAS_QUIC

#include "session.h"

namespace node {
namespace quic {

std::unique_ptr<Session::Application> createHttp3Application(
    Session* session, const Session::Application_Options& options);

}  // namespace quic
}  // namespace node

#endif  // HAVE_OPENSSL && NODE_OPENSSL_HAS_QUIC
#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS
