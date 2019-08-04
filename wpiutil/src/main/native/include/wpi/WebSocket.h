/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_WEBSOCKET_H_
#define WPIUTIL_WPI_WEBSOCKET_H_

#include <stdint.h>

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <utility>

#include "wpi/ArrayRef.h"
#include "wpi/Signal.h"
#include "wpi/SmallVector.h"
#include "wpi/StringRef.h"
#include "wpi/Twine.h"
#include "wpi/uv/Buffer.h"
#include "wpi/uv/Error.h"
#include "wpi/uv/Timer.h"

namespace wpi {

namespace uv {
class Stream;
}  // namespace uv

/**
 * RFC 6455 compliant WebSocket client and server implementation.
 */
class WebSocket : public std::enable_shared_from_this<WebSocket> {
  struct private_init {};

  static constexpr uint8_t kOpCont = 0x00;
  static constexpr uint8_t kOpText = 0x01;
  static constexpr uint8_t kOpBinary = 0x02;
  static constexpr uint8_t kOpClose = 0x08;
  static constexpr uint8_t kOpPing = 0x09;
  static constexpr uint8_t kOpPong = 0x0A;
  static constexpr uint8_t kOpMask = 0x0F;
  static constexpr uint8_t kFlagFin = 0x80;
  static constexpr uint8_t kFlagMasking = 0x80;
  static constexpr uint8_t kLenMask = 0x7f;

 public:
  WebSocket(uv::Stream& stream, bool server, const private_init&);
  WebSocket(const WebSocket&) = delete;
  WebSocket(WebSocket&&) = delete;
  WebSocket& operator=(const WebSocket&) = delete;
  WebSocket& operator=(WebSocket&&) = delete;
  ~WebSocket();

  /**
   * Connection states.
   */
  enum State {
    /** The connection is not yet open. */
    CONNECTING = 0,
    /** The connection is open and ready to communicate. */
    OPEN,
    /** The connection is in the process of closing. */
    CLOSING,
    /** The connection failed. */
    FAILED,
    /** The connection is closed. */
    CLOSED
  };

  /**
   * Client connection options.
   */
  struct ClientOptions {
    ClientOptions() : handshakeTimeout{(uv::Timer::Time::max)()} {}

    /** Timeout for the handshake request. */
    uv::Timer::Time handshakeTimeout;

    /** Additional headers to include in handshake. */
    ArrayRef<std::pair<StringRef, StringRef>> extraHeaders;
  };

  /**
   * Starts a client connection by performing the initial client handshake.
   * An open event is emitted when the handshake completes.
   * This sets the stream user data to the websocket.
   * @param stream Connection stream
   * @param uri The Request-URI to send
   * @param host The host or host:port to send
   * @param protocols The list of subprotocols
   * @param options Handshake options
   */
  static std::shared_ptr<WebSocket> CreateClient(
      uv::Stream& stream, const Twine& uri, const Twine& host,
      ArrayRef<StringRef> protocols = ArrayRef<StringRef>{},
      const ClientOptions& options = ClientOptions{});

  /**
   * Starts a client connection by performing the initial client handshake.
   * An open event is emitted when the handshake completes.
   * This sets the stream user data to the websocket.
   * @param stream Connection stream
   * @param uri The Request-URI to send
   * @param host The host or host:port to send
   * @param protocols The list of subprotocols
   * @param options Handshake options
   */
  static std::shared_ptr<WebSocket> CreateClient(
      uv::Stream& stream, const Twine& uri, const Twine& host,
      std::initializer_list<StringRef> protocols,
      const ClientOptions& options = ClientOptions{}) {
    return CreateClient(stream, uri, host,
                        makeArrayRef(protocols.begin(), protocols.end()),
                        options);
  }

  /**
   * Starts a server connection by performing the initial server side handshake.
   * This should be called after the HTTP headers have been received.
   * An open event is emitted when the handshake completes.
   * This sets the stream user data to the websocket.
   * @param stream Connection stream
   * @param key The value of the Sec-WebSocket-Key header field in the client
   *            request
   * @param version The value of the Sec-WebSocket-Version header field in the
   *                client request
   * @param protocol The subprotocol to send to the client (in the
   *                 Sec-WebSocket-Protocol header field).
   */
  static std::shared_ptr<WebSocket> CreateServer(
      uv::Stream& stream, StringRef key, StringRef version,
      StringRef protocol = StringRef{});

  /**
   * Get connection state.
   */
  State GetState() const { return m_state; }

  /**
   * Return if the connection is open.  Messages can only be sent on open
   * connections.
   */
  bool IsOpen() const { return m_state == OPEN; }

  /**
   * Get the underlying stream.
   */
  uv::Stream& GetStream() const { return m_stream; }

  /**
   * Get the selected sub-protocol.  Only valid in or after the open() event.
   */
  StringRef GetProtocol() const { return m_protocol; }

  /**
   * Set the maximum message size.  Default is 128 KB.  If configured to combine
   * fragments this maximum applies to the entire message (all combined
   * fragments).
   * @param size Maximum message size in bytes
   */
  void SetMaxMessageSize(size_t size) { m_maxMessageSize = size; }

  /**
   * Set whether or not fragmented frames should be combined.  Default is to
   * combine.  If fragmented frames are combined, the text and binary callbacks
   * will always have the second parameter (fin) set to true.
   * @param combine True if fragmented frames should be combined.
   */
  void SetCombineFragments(bool combine) { m_combineFragments = combine; }

  /**
   * Initiate a closing handshake.
   * @param code A numeric status code (defaults to 1005, no status code)
   * @param reason A human-readable string explaining why the connection is
   *               closing (optional).
   */
  void Close(uint16_t code = 1005, const Twine& reason = Twine{});

  /**
   * Send a text message.
   * @param data UTF-8 encoded data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendText(
      ArrayRef<uv::Buffer> data,
      std::function<void(MutableArrayRef<uv::Buffer>, uv::Error)> callback) {
    Send(kFlagFin | kOpText, data, callback);
  }

  /**
   * Send a binary message.
   * @param data Data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendBinary(
      ArrayRef<uv::Buffer> data,
      std::function<void(MutableArrayRef<uv::Buffer>, uv::Error)> callback) {
    Send(kFlagFin | kOpBinary, data, callback);
  }

  /**
   * Send a text message fragment.  This must be followed by one or more
   * SendFragment() calls, where the last one has fin=True, to complete the
   * message.
   * @param data UTF-8 encoded data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendTextFragment(
      ArrayRef<uv::Buffer> data,
      std::function<void(MutableArrayRef<uv::Buffer>, uv::Error)> callback) {
    Send(kOpText, data, callback);
  }

  /**
   * Send a text message fragment.  This must be followed by one or more
   * SendFragment() calls, where the last one has fin=True, to complete the
   * message.
   * @param data Data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendBinaryFragment(
      ArrayRef<uv::Buffer> data,
      std::function<void(MutableArrayRef<uv::Buffer>, uv::Error)> callback) {
    Send(kOpBinary, data, callback);
  }

  /**
   * Send a continuation frame.  This is used to send additional parts of a
   * message started with SendTextFragment() or SendBinaryFragment().
   * @param data Data to send
   * @param fin Set to true if this is the final fragment of the message
   * @param callback Callback which is invoked when the write completes.
   */
  void SendFragment(
      ArrayRef<uv::Buffer> data, bool fin,
      std::function<void(MutableArrayRef<uv::Buffer>, uv::Error)> callback) {
    Send(kOpCont | (fin ? kFlagFin : 0), data, callback);
  }

  /**
   * Send a ping frame with no data.
   * @param callback Optional callback which is invoked when the ping frame
   *                 write completes.
   */
  void SendPing(std::function<void(uv::Error)> callback = nullptr) {
    SendPing(ArrayRef<uv::Buffer>{}, [callback](auto bufs, uv::Error err) {
      if (callback) callback(err);
    });
  }

  /**
   * Send a ping frame.
   * @param data Data to send in the ping frame
   * @param callback Callback which is invoked when the ping frame
   *                 write completes.
   */
  void SendPing(
      ArrayRef<uv::Buffer> data,
      std::function<void(MutableArrayRef<uv::Buffer>, uv::Error)> callback) {
    Send(kFlagFin | kOpPing, data, callback);
  }

  /**
   * Send a pong frame with no data.
   * @param callback Optional callback which is invoked when the pong frame
   *                 write completes.
   */
  void SendPong(std::function<void(uv::Error)> callback = nullptr) {
    SendPong(ArrayRef<uv::Buffer>{}, [callback](auto bufs, uv::Error err) {
      if (callback) callback(err);
    });
  }

  /**
   * Send a pong frame.
   * @param data Data to send in the pong frame
   * @param callback Callback which is invoked when the pong frame
   *                 write completes.
   */
  void SendPong(
      ArrayRef<uv::Buffer> data,
      std::function<void(MutableArrayRef<uv::Buffer>, uv::Error)> callback) {
    Send(kFlagFin | kOpPong, data, callback);
  }

  /**
   * Fail the connection.
   */
  void Fail(uint16_t code = 1002, const Twine& reason = "protocol error");

  /**
   * Forcibly close the connection.
   */
  void Terminate(uint16_t code = 1006, const Twine& reason = "terminated");

  /**
   * Gets user-defined data.
   * @return User-defined data if any, nullptr otherwise.
   */
  template <typename T = void>
  std::shared_ptr<T> GetData() const {
    return std::static_pointer_cast<T>(m_data);
  }

  /**
   * Sets user-defined data.
   * @param data User-defined arbitrary data.
   */
  void SetData(std::shared_ptr<void> data) { m_data = std::move(data); }

  /**
   * Open event.  Emitted when the connection is open and ready to communicate.
   * The parameter is the selected subprotocol.
   */
  sig::Signal<StringRef> open;

  /**
   * Close event.  Emitted when the connection is closed.  The first parameter
   * is a numeric value indicating the status code explaining why the connection
   * has been closed.  The second parameter is a human-readable string
   * explaining the reason why the connection has been closed.
   */
  sig::Signal<uint16_t, StringRef> closed;

  /**
   * Text message event.  Emitted when a text message is received.
   * The first parameter is the data, the second parameter is true if the
   * data is the last fragment of the message.
   */
  sig::Signal<StringRef, bool> text;

  /**
   * Binary message event.  Emitted when a binary message is received.
   * The first parameter is the data, the second parameter is true if the
   * data is the last fragment of the message.
   */
  sig::Signal<ArrayRef<uint8_t>, bool> binary;

  /**
   * Ping event.  Emitted when a ping message is received.
   */
  sig::Signal<ArrayRef<uint8_t>> ping;

  /**
   * Pong event.  Emitted when a pong message is received.
   */
  sig::Signal<ArrayRef<uint8_t>> pong;

 private:
  // user data
  std::shared_ptr<void> m_data;

  // constructor parameters
  uv::Stream& m_stream;
  bool m_server;

  // subprotocol, set via constructor (server) or handshake (client)
  std::string m_protocol;

  // user-settable configuration
  size_t m_maxMessageSize = 128 * 1024;
  bool m_combineFragments = true;

  // operating state
  State m_state = CONNECTING;

  // incoming message buffers/state
  SmallVector<uint8_t, 14> m_header;
  size_t m_headerSize = 0;
  SmallVector<uint8_t, 1024> m_payload;
  size_t m_frameStart = 0;
  uint64_t m_frameSize = UINT64_MAX;
  uint8_t m_fragmentOpcode = 0;

  // temporary data used only during client handshake
  class ClientHandshakeData;
  std::unique_ptr<ClientHandshakeData> m_clientHandshake;

  void StartClient(const Twine& uri, const Twine& host,
                   ArrayRef<StringRef> protocols, const ClientOptions& options);
  void StartServer(StringRef key, StringRef version, StringRef protocol);
  void SendClose(uint16_t code, const Twine& reason);
  void SetClosed(uint16_t code, const Twine& reason, bool failed = false);
  void Shutdown();
  void HandleIncoming(uv::Buffer& buf, size_t size);
  void Send(
      uint8_t opcode, ArrayRef<uv::Buffer> data,
      std::function<void(MutableArrayRef<uv::Buffer>, uv::Error)> callback);
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_WEBSOCKET_H_
