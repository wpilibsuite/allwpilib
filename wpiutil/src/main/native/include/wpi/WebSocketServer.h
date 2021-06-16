// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_WEBSOCKETSERVER_H_
#define WPIUTIL_WPI_WEBSOCKETSERVER_H_

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "wpi/HttpParser.h"
#include "wpi/Signal.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/WebSocket.h"
#include "wpi/span.h"

namespace wpi {

namespace uv {
class Stream;
}  // namespace uv

/**
 * WebSocket HTTP server helper.  Handles websocket-specific headers.  User
 * must provide the HttpParser.
 */
class WebSocketServerHelper {
 public:
  /**
   * Constructor.
   * @param req HttpParser for request
   */
  explicit WebSocketServerHelper(HttpParser& req);

  /**
   * Get whether or not this was a websocket upgrade.
   * Only valid during and after the upgrade event.
   */
  bool IsWebsocket() const { return m_websocket; }

  /**
   * Try to find a match to the list of sub-protocols provided by the client.
   * The list is priority ordered, so the first match wins.
   * Only valid during and after the upgrade event.
   * @param protocols Acceptable protocols
   * @return Pair; first item is true if a match was made, false if not.
   *         Second item is the matched protocol if a match was made, otherwise
   *         is empty.
   */
  std::pair<bool, std::string_view> MatchProtocol(
      span<const std::string_view> protocols);

  /**
   * Try to find a match to the list of sub-protocols provided by the client.
   * The list is priority ordered, so the first match wins.
   * Only valid during and after the upgrade event.
   * @param protocols Acceptable protocols
   * @return Pair; first item is true if a match was made, false if not.
   *         Second item is the matched protocol if a match was made, otherwise
   *         is empty.
   */
  std::pair<bool, std::string_view> MatchProtocol(
      std::initializer_list<std::string_view> protocols) {
    return MatchProtocol({protocols.begin(), protocols.end()});
  }

  /**
   * Accept the upgrade.  Disconnect other readers (such as the HttpParser
   * reader) before calling this.  See also WebSocket::CreateServer().
   * @param stream Connection stream
   * @param protocol The subprotocol to send to the client
   */
  std::shared_ptr<WebSocket> Accept(uv::Stream& stream,
                                    std::string_view protocol = {}) {
    return WebSocket::CreateServer(stream, m_key, m_version, protocol);
  }

  bool IsUpgrade() const { return m_gotHost && m_websocket; }

  /**
   * Upgrade event.  Call Accept() to accept the upgrade.
   */
  sig::Signal<> upgrade;

 private:
  bool m_gotHost = false;
  bool m_websocket = false;
  SmallVector<std::string, 2> m_protocols;
  SmallString<64> m_key;
  SmallString<16> m_version;
};

/**
 * Dedicated WebSocket server.
 */
class WebSocketServer : public std::enable_shared_from_this<WebSocketServer> {
  struct private_init {};

 public:
  /**
   * Server options.
   */
  struct ServerOptions {
    /**
     * Checker for URL.  Return true if URL should be accepted.  By default all
     * URLs are accepted.
     */
    std::function<bool(std::string_view)> checkUrl;

    /**
     * Checker for Host header.  Return true if Host should be accepted.  By
     * default all hosts are accepted.
     */
    std::function<bool(std::string_view)> checkHost;
  };

  /**
   * Private constructor.
   */
  WebSocketServer(uv::Stream& stream, span<const std::string_view> protocols,
                  ServerOptions options, const private_init&);

  /**
   * Starts a dedicated WebSocket server on the provided connection.  The
   * connection should be an accepted client stream.
   * This also sets the stream user data to the socket server.
   * A connected event is emitted when the connection is opened.
   * @param stream Connection stream
   * @param protocols Acceptable subprotocols
   * @param options Handshake options
   */
  static std::shared_ptr<WebSocketServer> Create(
      uv::Stream& stream, span<const std::string_view> protocols = {},
      const ServerOptions& options = {});

  /**
   * Starts a dedicated WebSocket server on the provided connection.  The
   * connection should be an accepted client stream.
   * This also sets the stream user data to the socket server.
   * A connected event is emitted when the connection is opened.
   * @param stream Connection stream
   * @param protocols Acceptable subprotocols
   * @param options Handshake options
   */
  static std::shared_ptr<WebSocketServer> Create(
      uv::Stream& stream, std::initializer_list<std::string_view> protocols,
      const ServerOptions& options = {}) {
    return Create(stream, {protocols.begin(), protocols.end()}, options);
  }

  /**
   * Connected event.  First parameter is the URL, second is the websocket.
   */
  sig::Signal<std::string_view, WebSocket&> connected;

 private:
  uv::Stream& m_stream;
  HttpParser m_req{HttpParser::kRequest};
  WebSocketServerHelper m_helper;
  SmallVector<std::string, 2> m_protocols;
  ServerOptions m_options;
  bool m_aborted = false;
  sig::ScopedConnection m_dataConn;
  sig::ScopedConnection m_errorConn;
  sig::ScopedConnection m_endConn;

  void Abort(uint16_t code, std::string_view reason);
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_WEBSOCKETSERVER_H_
