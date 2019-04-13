/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/WebSocket.h"

#include <random>

#include "wpi/Base64.h"
#include "wpi/HttpParser.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/raw_uv_ostream.h"
#include "wpi/sha1.h"
#include "wpi/uv/Stream.h"

using namespace wpi;

namespace {
class WebSocketWriteReq : public uv::WriteReq {
 public:
  explicit WebSocketWriteReq(
      std::function<void(MutableArrayRef<uv::Buffer>, uv::Error)> callback) {
    finish.connect([=](uv::Error err) {
      MutableArrayRef<uv::Buffer> bufs{m_bufs};
      for (auto&& buf : bufs.slice(0, m_startUser)) buf.Deallocate();
      callback(bufs.slice(m_startUser), err);
    });
  }

  SmallVector<uv::Buffer, 4> m_bufs;
  size_t m_startUser;
};
}  // namespace

class WebSocket::ClientHandshakeData {
 public:
  ClientHandshakeData() {
    // key is a random nonce
    static std::random_device rd;
    static std::default_random_engine gen{rd()};
    std::uniform_int_distribution<unsigned int> dist(0, 255);
    char nonce[16];  // the nonce sent to the server
    for (char& v : nonce) v = static_cast<char>(dist(gen));
    raw_svector_ostream os(key);
    Base64Encode(os, StringRef{nonce, 16});
  }
  ~ClientHandshakeData() {
    if (auto t = timer.lock()) {
      t->Stop();
      t->Close();
    }
  }

  SmallString<64> key;                       // the key sent to the server
  SmallVector<std::string, 2> protocols;     // valid protocols
  HttpParser parser{HttpParser::kResponse};  // server response parser
  bool hasUpgrade = false;
  bool hasConnection = false;
  bool hasAccept = false;
  bool hasProtocol = false;

  std::weak_ptr<uv::Timer> timer;
};

static StringRef AcceptHash(StringRef key, SmallVectorImpl<char>& buf) {
  SHA1 hash;
  hash.Update(key);
  hash.Update("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
  SmallString<64> hashBuf;
  return Base64Encode(hash.RawFinal(hashBuf), buf);
}

WebSocket::WebSocket(uv::Stream& stream, bool server, const private_init&)
    : m_stream{stream}, m_server{server} {
  // Connect closed and error signals to ourselves
  m_stream.closed.connect([this]() { SetClosed(1006, "handle closed"); });
  m_stream.error.connect([this](uv::Error err) {
    Terminate(1006, "stream error: " + Twine(err.name()));
  });

  // Start reading
  m_stream.StopRead();  // we may have been reading
  m_stream.StartRead();
  m_stream.data.connect(
      [this](uv::Buffer& buf, size_t size) { HandleIncoming(buf, size); });
  m_stream.end.connect(
      [this]() { Terminate(1006, "remote end closed connection"); });
}

WebSocket::~WebSocket() {}

std::shared_ptr<WebSocket> WebSocket::CreateClient(
    uv::Stream& stream, const Twine& uri, const Twine& host,
    ArrayRef<StringRef> protocols, const ClientOptions& options) {
  auto ws = std::make_shared<WebSocket>(stream, false, private_init{});
  stream.SetData(ws);
  ws->StartClient(uri, host, protocols, options);
  return ws;
}

std::shared_ptr<WebSocket> WebSocket::CreateServer(uv::Stream& stream,
                                                   StringRef key,
                                                   StringRef version,
                                                   StringRef protocol) {
  auto ws = std::make_shared<WebSocket>(stream, true, private_init{});
  stream.SetData(ws);
  ws->StartServer(key, version, protocol);
  return ws;
}

void WebSocket::Close(uint16_t code, const Twine& reason) {
  SendClose(code, reason);
  if (m_state != FAILED && m_state != CLOSED) m_state = CLOSING;
}

void WebSocket::Fail(uint16_t code, const Twine& reason) {
  if (m_state == FAILED || m_state == CLOSED) return;
  SendClose(code, reason);
  SetClosed(code, reason, true);
  Shutdown();
}

void WebSocket::Terminate(uint16_t code, const Twine& reason) {
  if (m_state == FAILED || m_state == CLOSED) return;
  SetClosed(code, reason);
  Shutdown();
}

void WebSocket::StartClient(const Twine& uri, const Twine& host,
                            ArrayRef<StringRef> protocols,
                            const ClientOptions& options) {
  // Create client handshake data
  m_clientHandshake = std::make_unique<ClientHandshakeData>();

  // Build client request
  SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os{bufs, 4096};

  os << "GET " << uri << " HTTP/1.1\r\n";
  os << "Host: " << host << "\r\n";
  os << "Upgrade: websocket\r\n";
  os << "Connection: Upgrade\r\n";
  os << "Sec-WebSocket-Key: " << m_clientHandshake->key << "\r\n";
  os << "Sec-WebSocket-Version: 13\r\n";

  // protocols (if provided)
  if (!protocols.empty()) {
    os << "Sec-WebSocket-Protocol: ";
    bool first = true;
    for (auto protocol : protocols) {
      if (!first)
        os << ", ";
      else
        first = false;
      os << protocol;
      // also save for later checking against server response
      m_clientHandshake->protocols.emplace_back(protocol);
    }
    os << "\r\n";
  }

  // other headers
  for (auto&& header : options.extraHeaders)
    os << header.first << ": " << header.second << "\r\n";

  // finish headers
  os << "\r\n";

  // Send client request
  m_stream.Write(bufs, [](auto bufs, uv::Error) {
    for (auto& buf : bufs) buf.Deallocate();
  });

  // Set up client response handling
  m_clientHandshake->parser.status.connect([this](StringRef status) {
    unsigned int code = m_clientHandshake->parser.GetStatusCode();
    if (code != 101) Terminate(code, status);
  });
  m_clientHandshake->parser.header.connect(
      [this](StringRef name, StringRef value) {
        value = value.trim();
        if (name.equals_lower("upgrade")) {
          if (!value.equals_lower("websocket"))
            return Terminate(1002, "invalid upgrade response value");
          m_clientHandshake->hasUpgrade = true;
        } else if (name.equals_lower("connection")) {
          if (!value.equals_lower("upgrade"))
            return Terminate(1002, "invalid connection response value");
          m_clientHandshake->hasConnection = true;
        } else if (name.equals_lower("sec-websocket-accept")) {
          // Check against expected response
          SmallString<64> acceptBuf;
          if (!value.equals(AcceptHash(m_clientHandshake->key, acceptBuf)))
            return Terminate(1002, "invalid accept key");
          m_clientHandshake->hasAccept = true;
        } else if (name.equals_lower("sec-websocket-extensions")) {
          // No extensions are supported
          if (!value.empty()) return Terminate(1010, "unsupported extension");
        } else if (name.equals_lower("sec-websocket-protocol")) {
          // Make sure it was one of the provided protocols
          bool match = false;
          for (auto&& protocol : m_clientHandshake->protocols) {
            if (value.equals_lower(protocol)) {
              match = true;
              break;
            }
          }
          if (!match) return Terminate(1003, "unsupported protocol");
          m_clientHandshake->hasProtocol = true;
          m_protocol = value;
        }
      });
  m_clientHandshake->parser.headersComplete.connect([this](bool) {
    if (!m_clientHandshake->hasUpgrade || !m_clientHandshake->hasConnection ||
        !m_clientHandshake->hasAccept ||
        (!m_clientHandshake->hasProtocol &&
         !m_clientHandshake->protocols.empty())) {
      return Terminate(1002, "invalid response");
    }
    if (m_state == CONNECTING) {
      m_state = OPEN;
      open(m_protocol);
    }
  });

  // Start handshake timer if a timeout was specified
  if (options.handshakeTimeout != uv::Timer::Time::max()) {
    auto timer = uv::Timer::Create(m_stream.GetLoopRef());
    timer->timeout.connect(
        [this]() { Terminate(1006, "connection timed out"); });
    timer->Start(options.handshakeTimeout);
    m_clientHandshake->timer = timer;
  }
}

void WebSocket::StartServer(StringRef key, StringRef version,
                            StringRef protocol) {
  m_protocol = protocol;

  // Build server response
  SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os{bufs, 4096};

  // Handle unsupported version
  if (version != "13") {
    os << "HTTP/1.1 426 Upgrade Required\r\n";
    os << "Upgrade: WebSocket\r\n";
    os << "Sec-WebSocket-Version: 13\r\n\r\n";
    m_stream.Write(bufs, [this](auto bufs, uv::Error) {
      for (auto& buf : bufs) buf.Deallocate();
      // XXX: Should we support sending a new handshake on the same connection?
      // XXX: "this->" is required by GCC 5.5 (bug)
      this->Terminate(1003, "unsupported protocol version");
    });
    return;
  }

  os << "HTTP/1.1 101 Switching Protocols\r\n";
  os << "Upgrade: websocket\r\n";
  os << "Connection: Upgrade\r\n";

  // accept hash
  SmallString<64> acceptBuf;
  os << "Sec-WebSocket-Accept: " << AcceptHash(key, acceptBuf) << "\r\n";

  if (!protocol.empty()) os << "Sec-WebSocket-Protocol: " << protocol << "\r\n";

  // end headers
  os << "\r\n";

  // Send server response
  m_stream.Write(bufs, [this](auto bufs, uv::Error) {
    for (auto& buf : bufs) buf.Deallocate();
    if (m_state == CONNECTING) {
      m_state = OPEN;
      open(m_protocol);
    }
  });
}

void WebSocket::SendClose(uint16_t code, const Twine& reason) {
  SmallVector<uv::Buffer, 4> bufs;
  if (code != 1005) {
    raw_uv_ostream os{bufs, 4096};
    os << ArrayRef<uint8_t>{static_cast<uint8_t>((code >> 8) & 0xff),
                            static_cast<uint8_t>(code & 0xff)};
    reason.print(os);
  }
  Send(kFlagFin | kOpClose, bufs, [](auto bufs, uv::Error) {
    for (auto&& buf : bufs) buf.Deallocate();
  });
}

void WebSocket::SetClosed(uint16_t code, const Twine& reason, bool failed) {
  if (m_state == FAILED || m_state == CLOSED) return;
  m_state = failed ? FAILED : CLOSED;
  SmallString<64> reasonBuf;
  closed(code, reason.toStringRef(reasonBuf));
}

void WebSocket::Shutdown() {
  m_stream.Shutdown([this] { m_stream.Close(); });
}

void WebSocket::HandleIncoming(uv::Buffer& buf, size_t size) {
  // ignore incoming data if we're failed or closed
  if (m_state == FAILED || m_state == CLOSED) return;

  StringRef data{buf.base, size};

  // Handle connecting state (mainly on client)
  if (m_state == CONNECTING) {
    if (m_clientHandshake) {
      data = m_clientHandshake->parser.Execute(data);
      // check for parser failure
      if (m_clientHandshake->parser.HasError())
        return Terminate(1003, "invalid response");
      if (m_state != OPEN) return;  // not done with handshake yet

      // we're done with the handshake, so release its memory
      m_clientHandshake.reset();

      // fall through to process additional data after handshake
    } else {
      return Terminate(1003, "got data on server before response");
    }
  }

  // Message processing
  while (!data.empty()) {
    if (m_frameSize == UINT64_MAX) {
      // Need at least two bytes to determine header length
      if (m_header.size() < 2u) {
        size_t toCopy = std::min(2u - m_header.size(), data.size());
        m_header.append(data.bytes_begin(), data.bytes_begin() + toCopy);
        data = data.drop_front(toCopy);
        if (m_header.size() < 2u) return;  // need more data

        // Validate RSV bits are zero
        if ((m_header[0] & 0x70) != 0) return Fail(1002, "nonzero RSV");
      }

      // Once we have first two bytes, we can calculate the header size
      if (m_headerSize == 0) {
        m_headerSize = 2;
        uint8_t len = m_header[1] & kLenMask;
        if (len == 126)
          m_headerSize += 2;
        else if (len == 127)
          m_headerSize += 8;
        bool masking = (m_header[1] & kFlagMasking) != 0;
        if (masking) m_headerSize += 4;  // masking key
        // On server side, incoming messages MUST be masked
        // On client side, incoming messages MUST NOT be masked
        if (m_server && !masking) return Fail(1002, "client data not masked");
        if (!m_server && masking) return Fail(1002, "server data masked");
      }

      // Need to complete header to calculate message size
      if (m_header.size() < m_headerSize) {
        size_t toCopy = std::min(m_headerSize - m_header.size(), data.size());
        m_header.append(data.bytes_begin(), data.bytes_begin() + toCopy);
        data = data.drop_front(toCopy);
        if (m_header.size() < m_headerSize) return;  // need more data
      }

      if (m_header.size() >= m_headerSize) {
        // get payload length
        uint8_t len = m_header[1] & kLenMask;
        if (len == 126)
          m_frameSize = (static_cast<uint16_t>(m_header[2]) << 8) |
                        static_cast<uint16_t>(m_header[3]);
        else if (len == 127)
          m_frameSize = (static_cast<uint64_t>(m_header[2]) << 56) |
                        (static_cast<uint64_t>(m_header[3]) << 48) |
                        (static_cast<uint64_t>(m_header[4]) << 40) |
                        (static_cast<uint64_t>(m_header[5]) << 32) |
                        (static_cast<uint64_t>(m_header[6]) << 24) |
                        (static_cast<uint64_t>(m_header[7]) << 16) |
                        (static_cast<uint64_t>(m_header[8]) << 8) |
                        static_cast<uint64_t>(m_header[9]);
        else
          m_frameSize = len;

        // limit maximum size
        if ((m_payload.size() + m_frameSize) > m_maxMessageSize)
          return Fail(1009, "message too large");
      }
    }

    if (m_frameSize != UINT64_MAX) {
      size_t need = m_frameStart + m_frameSize - m_payload.size();
      size_t toCopy = std::min(need, data.size());
      m_payload.append(data.bytes_begin(), data.bytes_begin() + toCopy);
      data = data.drop_front(toCopy);
      need -= toCopy;
      if (need == 0) {
        // We have a complete frame
        // If the message had masking, unmask it
        if ((m_header[1] & kFlagMasking) != 0) {
          uint8_t key[4] = {
              m_header[m_headerSize - 4], m_header[m_headerSize - 3],
              m_header[m_headerSize - 2], m_header[m_headerSize - 1]};
          int n = 0;
          for (uint8_t& ch :
               MutableArrayRef<uint8_t>{m_payload}.slice(m_frameStart)) {
            ch ^= key[n++];
            if (n >= 4) n = 0;
          }
        }

        // Handle message
        bool fin = (m_header[0] & kFlagFin) != 0;
        uint8_t opcode = m_header[0] & kOpMask;
        switch (opcode) {
          case kOpCont:
            switch (m_fragmentOpcode) {
              case kOpText:
                if (!m_combineFragments || fin)
                  text(StringRef{reinterpret_cast<char*>(m_payload.data()),
                                 m_payload.size()},
                       fin);
                break;
              case kOpBinary:
                if (!m_combineFragments || fin) binary(m_payload, fin);
                break;
              default:
                // no preceding message?
                return Fail(1002, "invalid continuation message");
            }
            if (fin) m_fragmentOpcode = 0;
            break;
          case kOpText:
            if (m_fragmentOpcode != 0) return Fail(1002, "incomplete fragment");
            if (!m_combineFragments || fin)
              text(StringRef{reinterpret_cast<char*>(m_payload.data()),
                             m_payload.size()},
                   fin);
            if (!fin) m_fragmentOpcode = opcode;
            break;
          case kOpBinary:
            if (m_fragmentOpcode != 0) return Fail(1002, "incomplete fragment");
            if (!m_combineFragments || fin) binary(m_payload, fin);
            if (!fin) m_fragmentOpcode = opcode;
            break;
          case kOpClose: {
            uint16_t code;
            StringRef reason;
            if (!fin) {
              code = 1002;
              reason = "cannot fragment control frames";
            } else if (m_payload.size() < 2) {
              code = 1005;
            } else {
              code = (static_cast<uint16_t>(m_payload[0]) << 8) |
                     static_cast<uint16_t>(m_payload[1]);
              reason = StringRef{reinterpret_cast<char*>(m_payload.data()),
                                 m_payload.size()}
                           .drop_front(2);
            }
            // Echo the close if we didn't previously send it
            if (m_state != CLOSING) SendClose(code, reason);
            SetClosed(code, reason);
            // If we're the server, shutdown the connection.
            if (m_server) Shutdown();
            break;
          }
          case kOpPing:
            if (!fin) return Fail(1002, "cannot fragment control frames");
            ping(m_payload);
            break;
          case kOpPong:
            if (!fin) return Fail(1002, "cannot fragment control frames");
            pong(m_payload);
            break;
          default:
            return Fail(1002, "invalid message opcode");
        }

        // Prepare for next message
        m_header.clear();
        m_headerSize = 0;
        if (!m_combineFragments || fin) m_payload.clear();
        m_frameStart = m_payload.size();
        m_frameSize = UINT64_MAX;
      }
    }
  }
}

void WebSocket::Send(
    uint8_t opcode, ArrayRef<uv::Buffer> data,
    std::function<void(MutableArrayRef<uv::Buffer>, uv::Error)> callback) {
  // If we're not open, emit an error and don't send the data
  if (m_state != OPEN) {
    int err;
    if (m_state == CONNECTING)
      err = UV_EAGAIN;
    else
      err = UV_ESHUTDOWN;
    SmallVector<uv::Buffer, 4> bufs{data.begin(), data.end()};
    callback(bufs, uv::Error{err});
    return;
  }

  auto req = std::make_shared<WebSocketWriteReq>(callback);
  raw_uv_ostream os{req->m_bufs, 4096};

  // opcode (includes FIN bit)
  os << static_cast<unsigned char>(opcode);

  // payload length
  uint64_t size = 0;
  for (auto&& buf : data) size += buf.len;
  if (size < 126) {
    os << static_cast<unsigned char>((m_server ? 0x00 : kFlagMasking) | size);
  } else if (size <= 0xffff) {
    os << static_cast<unsigned char>((m_server ? 0x00 : kFlagMasking) | 126);
    os << ArrayRef<uint8_t>{static_cast<uint8_t>((size >> 8) & 0xff),
                            static_cast<uint8_t>(size & 0xff)};
  } else {
    os << static_cast<unsigned char>((m_server ? 0x00 : kFlagMasking) | 127);
    os << ArrayRef<uint8_t>{static_cast<uint8_t>((size >> 56) & 0xff),
                            static_cast<uint8_t>((size >> 48) & 0xff),
                            static_cast<uint8_t>((size >> 40) & 0xff),
                            static_cast<uint8_t>((size >> 32) & 0xff),
                            static_cast<uint8_t>((size >> 24) & 0xff),
                            static_cast<uint8_t>((size >> 16) & 0xff),
                            static_cast<uint8_t>((size >> 8) & 0xff),
                            static_cast<uint8_t>(size & 0xff)};
  }

  // clients need to mask the input data
  if (!m_server) {
    // generate masking key
    static std::random_device rd;
    static std::default_random_engine gen{rd()};
    std::uniform_int_distribution<unsigned int> dist(0, 255);
    uint8_t key[4];
    for (uint8_t& v : key) v = dist(gen);
    os << ArrayRef<uint8_t>{key, 4};
    // copy and mask data
    int n = 0;
    for (auto&& buf : data) {
      for (auto&& ch : buf.data()) {
        os << static_cast<unsigned char>(static_cast<uint8_t>(ch) ^ key[n++]);
        if (n >= 4) n = 0;
      }
    }
    req->m_startUser = req->m_bufs.size();
    req->m_bufs.append(data.begin(), data.end());
    // don't send the user bufs as we copied their data
    m_stream.Write(ArrayRef<uv::Buffer>{req->m_bufs}.slice(0, req->m_startUser),
                   req);
  } else {
    // servers can just send the buffers directly without masking
    req->m_startUser = req->m_bufs.size();
    req->m_bufs.append(data.begin(), data.end());
    m_stream.Write(req->m_bufs, req);
  }
}
