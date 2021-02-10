// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_WEBSOCKETSERVER_H_
#define WPIUTIL_WPI_WEBSOCKETSERVER_H_

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <utility>

#include "wpi/ArrayRef.h"
#include "wpi/HttpParser.h"
#include "wpi/Signal.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/StringRef.h"
#include "wpi/WebSocket.h"

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
  std::pair<bool, StringRef> MatchProtocol(ArrayRef<StringRef> protocols);

  /**
   * Try to find a match to the list of sub-protocols provided by the client.
   * The list is priority ordered, so the first match wins.
   * Only valid during and after the upgrade event.
   * @param protocols Acceptable protocols
   * @return Pair; first item is true if a match was made, false if not.
   *         Second item is the matched protocol if a match was made, otherwise
   *         is empty.
   */
  std::pair<bool, StringRef> MatchProtocol(
      std::initializer_list<StringRef> protocols) {
    return MatchProtocol(makeArrayRef(protocols.begin(), protocols.end()));
  }

  /**
   * Accept the upgrade.  Disconnect other readers (such as the HttpParser
   * reader) before calling this.  See also WebSocket::CreateServer().
   * @param stream Connection stream
   * @param protocol The subprotocol to send to the client
   */
  std::shared_ptr<WebSocket> Accept(uv::Stream& stream,
                                    StringRef protocol = StringRef{}) {
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
    std::function<bool(StringRef)> checkUrl;

    /**
     * Checker for Host header.  Return true if Host should be accepted.  By
     * default all hosts are accepted.
     */
    std::function<bool(StringRef)> checkHost;
  };

  /**
   * Private constructor.
   */
  WebSocketServer(uv::Stream& stream, ArrayRef<StringRef> protocols,
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
      uv::Stream& stream, ArrayRef<StringRef> protocols = ArrayRef<StringRef>{},
      const ServerOptions& options = ServerOptions{});

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
      uv::Stream& stream, std::initializer_list<StringRef> protocols,
      const ServerOptions& options = ServerOptions{}) {
    return Create(stream, makeArrayRef(protocols.begin(), protocols.end()),
                  options);
  }

  /**
   * Connected event.  First parameter is the URL, second is the websocket.
   */
  sig::Signal<StringRef, WebSocket&> connected;

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

  void Abort(uint16_t code, StringRef reason);
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_WEBSOCKETSERVER_H_
