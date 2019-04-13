/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/WebSocketServer.h"

#include "wpi/raw_uv_ostream.h"
#include "wpi/uv/Buffer.h"
#include "wpi/uv/Stream.h"

using namespace wpi;

WebSocketServerHelper::WebSocketServerHelper(HttpParser& req) {
  req.header.connect([this](StringRef name, StringRef value) {
    if (name.equals_lower("host")) {
      m_gotHost = true;
    } else if (name.equals_lower("upgrade")) {
      if (value.equals_lower("websocket")) m_websocket = true;
    } else if (name.equals_lower("sec-websocket-key")) {
      m_key = value;
    } else if (name.equals_lower("sec-websocket-version")) {
      m_version = value;
    } else if (name.equals_lower("sec-websocket-protocol")) {
      // Protocols are comma delimited, repeated headers add to list
      SmallVector<StringRef, 2> protocols;
      value.split(protocols, ",", -1, false);
      for (auto protocol : protocols) {
        protocol = protocol.trim();
        if (!protocol.empty()) m_protocols.emplace_back(protocol);
      }
    }
  });
  req.headersComplete.connect([&req, this](bool) {
    if (req.IsUpgrade() && IsUpgrade()) upgrade();
  });
}

std::pair<bool, StringRef> WebSocketServerHelper::MatchProtocol(
    ArrayRef<StringRef> protocols) {
  if (protocols.empty() && m_protocols.empty())
    return std::make_pair(true, StringRef{});
  for (auto protocol : protocols) {
    for (auto&& clientProto : m_protocols) {
      if (protocol == clientProto) return std::make_pair(true, protocol);
    }
  }
  return std::make_pair(false, StringRef{});
}

WebSocketServer::WebSocketServer(uv::Stream& stream,
                                 ArrayRef<StringRef> protocols,
                                 const ServerOptions& options,
                                 const private_init&)
    : m_stream{stream},
      m_helper{m_req},
      m_protocols{protocols.begin(), protocols.end()},
      m_options{options} {
  // Header handling
  m_req.header.connect([this](StringRef name, StringRef value) {
    if (name.equals_lower("host")) {
      if (m_options.checkHost) {
        if (!m_options.checkHost(value)) Abort(401, "Unrecognized Host");
      }
    }
  });
  m_req.url.connect([this](StringRef name) {
    if (m_options.checkUrl) {
      if (!m_options.checkUrl(name)) Abort(404, "Not Found");
    }
  });
  m_req.headersComplete.connect([this](bool) {
    // We only accept websocket connections
    if (!m_helper.IsUpgrade() || !m_req.IsUpgrade())
      Abort(426, "Upgrade Required");
  });

  // Handle upgrade event
  m_helper.upgrade.connect([this] {
    if (m_aborted) return;

    // Negotiate sub-protocol
    SmallVector<StringRef, 2> protocols{m_protocols.begin(), m_protocols.end()};
    StringRef protocol = m_helper.MatchProtocol(protocols).second;

    // Disconnect our header reader
    m_dataConn.disconnect();

    // Accepting the stream may destroy this (as it replaces the stream user
    // data), so grab a shared pointer first.
    auto self = shared_from_this();

    // Accept the upgrade
    auto ws = m_helper.Accept(m_stream, protocol);

    // Connect the websocket open event to our connected event.
    ws->open.connect_extended([ self, s = ws.get() ](auto conn, StringRef) {
      self->connected(self->m_req.GetUrl(), *s);
      conn.disconnect();  // one-shot
    });
  });

  // Set up stream
  stream.StartRead();
  m_dataConn =
      stream.data.connect_connection([this](uv::Buffer& buf, size_t size) {
        if (m_aborted) return;
        m_req.Execute(StringRef{buf.base, size});
        if (m_req.HasError()) Abort(400, "Bad Request");
      });
  m_errorConn =
      stream.error.connect_connection([this](uv::Error) { m_stream.Close(); });
  m_endConn = stream.end.connect_connection([this] { m_stream.Close(); });
}

std::shared_ptr<WebSocketServer> WebSocketServer::Create(
    uv::Stream& stream, ArrayRef<StringRef> protocols,
    const ServerOptions& options) {
  auto server = std::make_shared<WebSocketServer>(stream, protocols, options,
                                                  private_init{});
  stream.SetData(server);
  return server;
}

void WebSocketServer::Abort(uint16_t code, StringRef reason) {
  if (m_aborted) return;
  m_aborted = true;

  // Build response
  SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os{bufs, 1024};

  // Handle unsupported version
  os << "HTTP/1.1 " << code << ' ' << reason << "\r\n";
  if (code == 426) os << "Upgrade: WebSocket\r\n";
  os << "\r\n";
  m_stream.Write(bufs, [this](auto bufs, uv::Error) {
    for (auto& buf : bufs) buf.Deallocate();
    m_stream.Shutdown([this] { m_stream.Close(); });
  });
}
