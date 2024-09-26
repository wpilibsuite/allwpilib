// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/WebSocketServer.h"

#include <memory>
#include <utility>

#include <wpi/StringExtras.h>
#include <wpi/fmt/raw_ostream.h>
#include <wpi/print.h>

#include "wpinet/raw_uv_ostream.h"
#include "wpinet/uv/Buffer.h"
#include "wpinet/uv/Stream.h"

using namespace wpi;

WebSocketServerHelper::WebSocketServerHelper(HttpParser& req) {
  req.header.connect([this](std::string_view name, std::string_view value) {
    if (equals_lower(name, "host")) {
      m_gotHost = true;
    } else if (equals_lower(name, "upgrade")) {
      if (equals_lower(value, "websocket")) {
        m_websocket = true;
      }
    } else if (equals_lower(name, "sec-websocket-key")) {
      m_key = value;
    } else if (equals_lower(name, "sec-websocket-version")) {
      m_version = value;
    } else if (equals_lower(name, "sec-websocket-protocol")) {
      // Protocols are comma delimited, repeated headers add to list
      SmallVector<std::string_view, 2> protocols;
      split(value, protocols, ",", -1, false);
      for (auto protocol : protocols) {
        protocol = trim(protocol);
        if (!protocol.empty()) {
          m_protocols.emplace_back(protocol);
        }
      }
    }
  });
  req.headersComplete.connect([&req, this](bool) {
    if (req.IsUpgrade() && IsUpgrade()) {
      upgrade();
    }
  });
}

std::pair<bool, std::string_view> WebSocketServerHelper::MatchProtocol(
    std::span<const std::string_view> protocols) {
  if (protocols.empty() && m_protocols.empty()) {
    return {true, {}};
  }
  for (auto protocol : protocols) {
    for (auto&& clientProto : m_protocols) {
      if (protocol == clientProto) {
        return {true, protocol};
      }
    }
  }
  return {false, {}};
}

WebSocketServer::WebSocketServer(uv::Stream& stream,
                                 std::span<const std::string_view> protocols,
                                 ServerOptions options, const private_init&)
    : m_stream{stream},
      m_helper{m_req},
      m_protocols{protocols.begin(), protocols.end()},
      m_options{std::move(options)} {
  // Header handling
  m_req.header.connect([this](std::string_view name, std::string_view value) {
    if (equals_lower(name, "host")) {
      if (m_options.checkHost) {
        if (!m_options.checkHost(value)) {
          Abort(401, "Unrecognized Host");
        }
      }
    }
  });
  m_req.url.connect([this](std::string_view name) {
    if (m_options.checkUrl) {
      if (!m_options.checkUrl(name)) {
        Abort(404, "Not Found");
      }
    }
  });
  m_req.headersComplete.connect([this](bool) {
    // We only accept websocket connections
    if (!m_helper.IsUpgrade() || !m_req.IsUpgrade()) {
      Abort(426, "Upgrade Required");
    }
  });

  // Handle upgrade event
  m_helper.upgrade.connect([this] {
    if (m_aborted) {
      return;
    }

    // Negotiate sub-protocol
    SmallVector<std::string_view, 2> protocols{m_protocols.begin(),
                                               m_protocols.end()};
    std::string_view protocol = m_helper.MatchProtocol(protocols).second;

    // Disconnect our header reader
    m_dataConn.disconnect();

    // Accepting the stream may destroy this (as it replaces the stream user
    // data), so grab a shared pointer first.
    auto self = shared_from_this();

    // Accept the upgrade
    auto ws = m_helper.Accept(m_stream, protocol);

    // Connect the websocket open event to our connected event.
    ws->open.connect_extended(
        [self, s = ws.get()](auto conn, std::string_view) {
          self->connected(self->m_req.GetUrl(), *s);
          conn.disconnect();  // one-shot
        });
  });

  // Set up stream
  stream.StartRead();
  m_dataConn =
      stream.data.connect_connection([this](uv::Buffer& buf, size_t size) {
        if (m_aborted) {
          return;
        }
        m_req.Execute(std::string_view{buf.base, size});
        if (m_req.HasError()) {
          Abort(400, "Bad Request");
        }
      });
  m_errorConn =
      stream.error.connect_connection([this](uv::Error) { m_stream.Close(); });
  m_endConn = stream.end.connect_connection([this] { m_stream.Close(); });
}

std::shared_ptr<WebSocketServer> WebSocketServer::Create(
    uv::Stream& stream, std::span<const std::string_view> protocols,
    const ServerOptions& options) {
  auto server = std::make_shared<WebSocketServer>(stream, protocols, options,
                                                  private_init{});
  stream.SetData(server);
  return server;
}

void WebSocketServer::Abort(uint16_t code, std::string_view reason) {
  if (m_aborted) {
    return;
  }
  m_aborted = true;

  // Build response
  SmallVector<uv::Buffer, 4> bufs;
  raw_uv_ostream os{bufs, 1024};

  // Handle unsupported version
  wpi::print(os, "HTTP/1.1 {} {}\r\n", code, reason);
  if (code == 426) {
    os << "Upgrade: WebSocket\r\n";
  }
  os << "\r\n";
  m_stream.Write(bufs, [this](auto bufs, uv::Error) {
    for (auto& buf : bufs) {
      buf.Deallocate();
    }
    m_stream.Shutdown([this] { m_stream.Close(); });
  });
}
