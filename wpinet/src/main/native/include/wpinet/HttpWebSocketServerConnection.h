// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_HTTPWEBSOCKETSERVERCONNECTION_H_
#define WPINET_HTTPWEBSOCKETSERVERCONNECTION_H_

#include <initializer_list>
#include <memory>
#include <span>
#include <string>
#include <string_view>

#include <wpi/SmallVector.h>

#include "wpinet/HttpServerConnection.h"
#include "wpinet/WebSocket.h"
#include "wpinet/WebSocketServer.h"
#include "wpinet/uv/Stream.h"

namespace wpi {

/**
 * A server-side HTTP connection that also accepts WebSocket upgrades.
 *
 * @tparam Derived derived class for std::enable_shared_from_this.
 */
template <typename Derived>
class HttpWebSocketServerConnection
    : public HttpServerConnection,
      public std::enable_shared_from_this<Derived> {
 public:
  /**
   * Constructor.
   *
   * @param stream network stream
   * @param protocols Acceptable subprotocols
   */
  HttpWebSocketServerConnection(std::shared_ptr<uv::Stream> stream,
                                std::span<const std::string_view> protocols)
      : HttpServerConnection{stream},
        m_helper{m_request},
        m_protocols{protocols.begin(), protocols.end()} {
    // Handle upgrade event
    m_helper.upgrade.connect([this] {
      // Negotiate sub-protocol
      SmallVector<std::string_view, 2> protocols{m_protocols.begin(),
                                                 m_protocols.end()};
      std::string_view protocol = m_helper.MatchProtocol(protocols).second;

      // Check that the upgrade is valid
      if (!IsValidWsUpgrade(protocol)) {
        return;
      }

      // Disconnect HttpServerConnection header reader
      m_dataConn.disconnect();
      m_messageCompleteConn.disconnect();

      // Accepting the stream may destroy this (as it replaces the stream user
      // data), so grab a shared pointer first.
      auto self = this->shared_from_this();

      // Accept the upgrade
      auto ws = m_helper.Accept(m_stream, protocol);

      // Set this as the websocket user data to keep it around
      ws->SetData(self);

      // Store in member
      m_websocket = ws.get();

      // Call derived class function
      ProcessWsUpgrade();
    });
  }

  /**
   * Constructor.
   *
   * @param stream network stream
   * @param protocols Acceptable subprotocols
   */
  HttpWebSocketServerConnection(
      std::shared_ptr<uv::Stream> stream,
      std::initializer_list<std::string_view> protocols)
      : HttpWebSocketServerConnection(stream,
                                      {protocols.begin(), protocols.end()}) {}

 protected:
  /**
   * Check that an incoming WebSocket upgrade is okay.  This is called prior
   * to accepting the upgrade (so prior to ProcessWsUpgrade()).
   *
   * The implementation should check other headers and return true if the
   * WebSocket connection should be accepted.
   *
   * @param protocol negotiated subprotocol
   */
  virtual bool IsValidWsUpgrade(std::string_view protocol) { return true; }

  /**
   * Process an incoming WebSocket upgrade.  This is called after the header
   * reader has been disconnected and the websocket has been accepted.
   *
   * The implementation should set up appropriate callbacks on the websocket
   * object to continue communication.
   *
   * @note When a WebSocket upgrade occurs, the stream user data is replaced
   *       with the websocket, and the websocket user data points to "this".
   *       Replace the websocket user data with caution!
   */
  virtual void ProcessWsUpgrade() = 0;

  /**
   * WebSocket connection; not valid until ProcessWsUpgrade is called.
   */
  WebSocket* m_websocket = nullptr;

 private:
  WebSocketServerHelper m_helper;
  SmallVector<std::string, 2> m_protocols;
};

}  // namespace wpi

#endif  // WPINET_HTTPWEBSOCKETSERVERCONNECTION_H_
