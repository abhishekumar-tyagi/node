#include "node_crypto_clienthello.h"
#include "node_crypto_clienthello-inl.h"
#include "node_buffer.h"  // Buffer

// FUTURE TODO export maximum TLS ticket size:
static const size_t kTLSTicketSizeMask = 0xFFFF;

namespace node {

void ClientHelloParser::Parse(const uint8_t* data, size_t avail) {
  switch (state_) {
    case kWaiting:
      if (!ParseRecordHeader(data, avail))
        break;
      // Fall through
    case kTLSHeader:
      ParseHeader(data, avail);
      break;
    case kPaused:
      // Just nop
    case kEnded:
      // Already ended, just ignore it
      break;
    default:
      break;
  }
}


bool ClientHelloParser::ParseRecordHeader(const uint8_t* data, size_t avail) {
  // >= 5 bytes for header parsing
  if (avail < 5)
    return false;

  if (data[0] == kChangeCipherSpec ||
      data[0] == kAlert ||
      data[0] == kHandshake ||
      data[0] == kApplicationData) {
    frame_len_ = (data[3] << 8) + data[4];
    state_ = kTLSHeader;
    body_offset_ = 5;
  } else {
    End();
    return false;
  }

  // Sanity check (too big frame, or too small)
  // Let OpenSSL handle it
  if (frame_len_ >= kMaxTLSFrameLen) {
    End();
    return false;
  }

  return true;
}


void ClientHelloParser::ParseHeader(const uint8_t* data, size_t avail) {
  ClientHello hello;

  // >= 5 + frame size bytes for frame parsing
  if (body_offset_ + frame_len_ > avail)
    return;

  // Check hello protocol version.  Protocol tuples that we know about:
  //
  // (3,1) TLS v1.0
  // (3,2) TLS v1.1
  // (3,3) TLS v1.2
  //
  if (data[body_offset_ + 4] != 0x03 ||
      data[body_offset_ + 5] < 0x01 ||
      data[body_offset_ + 5] > 0x03) {
    goto fail;
  }

  if (data[body_offset_] == kClientHello) {
    if (state_ == kTLSHeader) {
      if (!ParseTLSClientHello(data, avail))
        goto fail;
    } else {
      // We couldn't get here, but whatever
      goto fail;
    }

    // Check if we overflowed (do not reply with any private data)
    if (session_id_ == nullptr ||
        session_size_ > 32 ||
        session_id_ + session_size_ > data + avail) {
      goto fail;
    }
  }

  state_ = kPaused;
  hello.session_id_ = session_id_;
  hello.session_size_ = session_size_;
  hello.has_ticket_ = tls_ticket_ != nullptr && tls_ticket_size_ != 0;
  hello.ocsp_request_ = ocsp_request_;
  hello.servername_ = servername_;
  hello.servername_size_ = static_cast<uint8_t>(servername_size_);
  onhello_cb_(cb_arg_, hello);
  return;

 fail:
  return End();
}


void ClientHelloParser::ParseExtension(const uint16_t type,
                                       const uint8_t* data,
                                       size_t len) {
  // NOTE: In case of anything we're just returning back, ignoring the problem.
  // That's because we're heavily relying on OpenSSL to solve any problem with
  // incoming data.
  switch (type) {
    case kServerName:
      {
        if (len < 2)
          return;
        uint32_t server_names_len = (data[0] << 8) + data[1];
        if (server_names_len + 2 > len)
          return;
        for (size_t offset = 2; offset < 2 + server_names_len; ) {
          if (offset + 3 > len)
            return;
          uint8_t name_type = data[offset];
          if (name_type != kServernameHostname)
            return;
          uint16_t name_len = (data[offset + 1] << 8) + data[offset + 2];
          offset += 3;
          if (offset + name_len > len)
            return;
          servername_ = data + offset;
          servername_size_ = name_len;
          offset += name_len;
        }
      }
      break;
    case kStatusRequest:
      // We are ignoring any data, just indicating the presence of extension
      if (len < kMinStatusRequestSize)
        return;

      // Unknown type, ignore it
      if (data[0] != kStatusRequestOCSP)
        break;

      // Ignore extensions, they won't work with caching on backend anyway
      ocsp_request_ = 1;
      break;
    case kTLSSessionTicket:
      // TBD POSSIBLE DATA LOSS:
      tls_ticket_size_ = static_cast<uint16_t>(len & kTLSTicketSizeMask);
      tls_ticket_ = data + len;
      break;
    default:
      // Ignore
      break;
  }
}


bool ClientHelloParser::ParseTLSClientHello(const uint8_t* data, size_t avail) {
  const uint8_t* body;

  // Skip frame header, hello header, protocol version and random data
  size_t session_offset = body_offset_ + 4 + 2 + 32;

  if (session_offset + 1 >= avail)
    return false;

  body = data + session_offset;
  session_size_ = *body;
  session_id_ = body + 1;

  size_t cipher_offset = session_offset + 1 + session_size_;

  // Session OOB failure
  if (cipher_offset + 1 >= avail)
    return false;

  uint16_t cipher_len =
      (data[cipher_offset] << 8) + data[cipher_offset + 1];
  size_t comp_offset = cipher_offset + 2 + cipher_len;

  // Cipher OOB failure
  if (comp_offset >= avail)
    return false;

  uint8_t comp_len = data[comp_offset];
  size_t extension_offset = comp_offset + 1 + comp_len;

  // Compression OOB failure
  if (extension_offset > avail)
    return false;

  // No extensions present
  if (extension_offset == avail)
    return true;

  size_t ext_off = extension_offset + 2;

  // Parse known extensions
  while (ext_off < avail) {
    // Extension OOB
    if (ext_off + 4 > avail)
      return false;

    uint16_t ext_type = (data[ext_off] << 8) + data[ext_off + 1];
    uint16_t ext_len = (data[ext_off + 2] << 8) + data[ext_off + 3];
    ext_off += 4;

    // Extension OOB
    if (ext_off + ext_len > avail)
      return false;

    ParseExtension(ext_type,
                   data + ext_off,
                   ext_len);

    ext_off += ext_len;
  }

  // Extensions OOB failure
  if (ext_off > avail)
    return false;

  return true;
}

}  // namespace node
