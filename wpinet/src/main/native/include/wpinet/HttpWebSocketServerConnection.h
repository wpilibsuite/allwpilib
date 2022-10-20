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
                                std::span<const std::string_view> protocols);

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

#include "HttpWebSocketServerConnection.inc"

#endif  // WPINET_HTTPWEBSOCKETSERVERCONNECTION_H_
