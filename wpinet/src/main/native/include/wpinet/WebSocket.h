// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_WEBSOCKET_H_
#define WPINET_WEBSOCKET_H_

#include <stdint.h>

#include <functional>
#include <initializer_list>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include <wpi/Signal.h>
#include <wpi/SmallVector.h>

#include "wpinet/uv/Buffer.h"
#include "wpinet/uv/Error.h"
#include "wpinet/uv/Timer.h"

namespace wpi {

namespace uv {
class Stream;
}  // namespace uv

/**
 * RFC 6455 compliant WebSocket client and server implementation.
 */
class WebSocket : public std::enable_shared_from_this<WebSocket> {
  struct private_init {};

 public:
  static constexpr uint8_t kOpCont = 0x00;
  static constexpr uint8_t kOpText = 0x01;
  static constexpr uint8_t kOpBinary = 0x02;
  static constexpr uint8_t kOpClose = 0x08;
  static constexpr uint8_t kOpPing = 0x09;
  static constexpr uint8_t kOpPong = 0x0A;
  static constexpr uint8_t kOpMask = 0x0F;
  static constexpr uint8_t kFlagFin = 0x80;
  static constexpr uint8_t kFlagControl = 0x08;

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
    uv::Timer::Time handshakeTimeout;  // NOLINT

    /** Additional headers to include in handshake. */
    std::span<const std::pair<std::string_view, std::string_view>> extraHeaders;
  };

  /**
   * Frame.  Used by SendFrames().
   */
  struct Frame {
    static constexpr uint8_t kText = kFlagFin | kOpText;
    static constexpr uint8_t kBinary = kFlagFin | kOpBinary;
    static constexpr uint8_t kTextFragment = kOpText;
    static constexpr uint8_t kBinaryFragment = kOpBinary;
    static constexpr uint8_t kFragment = kOpCont;
    static constexpr uint8_t kFinalFragment = kFlagFin | kOpCont;
    static constexpr uint8_t kPing = kFlagFin | kOpPing;
    static constexpr uint8_t kPong = kFlagFin | kOpPong;

    constexpr Frame(uint8_t opcode, std::span<const uv::Buffer> data)
        : opcode{opcode}, data{data} {}

    uint8_t opcode;
    std::span<const uv::Buffer> data;
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
      uv::Stream& stream, std::string_view uri, std::string_view host,
      std::span<const std::string_view> protocols = {},
      const ClientOptions& options = {});

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
      uv::Stream& stream, std::string_view uri, std::string_view host,
      std::initializer_list<std::string_view> protocols,
      const ClientOptions& options = {}) {
    return CreateClient(stream, uri, host, {protocols.begin(), protocols.end()},
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
      uv::Stream& stream, std::string_view key, std::string_view version,
      std::string_view protocol = {});

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
  std::string_view GetProtocol() const { return m_protocol; }

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
  void Close(uint16_t code = 1005, std::string_view reason = {});

  /**
   * Send a text message.
   * @param data UTF-8 encoded data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendText(
      std::span<const uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    Send(kFlagFin | kOpText, data, std::move(callback));
  }

  /**
   * Send a text message.
   * @param data UTF-8 encoded data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendText(
      std::initializer_list<uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    SendText({data.begin(), data.end()}, std::move(callback));
  }

  /**
   * Send a binary message.
   * @param data Data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendBinary(
      std::span<const uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    Send(kFlagFin | kOpBinary, data, std::move(callback));
  }

  /**
   * Send a binary message.
   * @param data Data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendBinary(
      std::initializer_list<uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    SendBinary({data.begin(), data.end()}, std::move(callback));
  }

  /**
   * Send a text message fragment.  This must be followed by one or more
   * SendFragment() calls, where the last one has fin=True, to complete the
   * message.
   * @param data UTF-8 encoded data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendTextFragment(
      std::span<const uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    Send(kOpText, data, std::move(callback));
  }

  /**
   * Send a text message fragment.  This must be followed by one or more
   * SendFragment() calls, where the last one has fin=True, to complete the
   * message.
   * @param data UTF-8 encoded data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendTextFragment(
      std::initializer_list<uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    SendTextFragment({data.begin(), data.end()}, std::move(callback));
  }

  /**
   * Send a text message fragment.  This must be followed by one or more
   * SendFragment() calls, where the last one has fin=True, to complete the
   * message.
   * @param data Data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendBinaryFragment(
      std::span<const uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    Send(kOpBinary, data, std::move(callback));
  }

  /**
   * Send a text message fragment.  This must be followed by one or more
   * SendFragment() calls, where the last one has fin=True, to complete the
   * message.
   * @param data Data to send
   * @param callback Callback which is invoked when the write completes.
   */
  void SendBinaryFragment(
      std::initializer_list<uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    SendBinaryFragment({data.begin(), data.end()}, std::move(callback));
  }

  /**
   * Send a continuation frame.  This is used to send additional parts of a
   * message started with SendTextFragment() or SendBinaryFragment().
   * @param data Data to send
   * @param fin Set to true if this is the final fragment of the message
   * @param callback Callback which is invoked when the write completes.
   */
  void SendFragment(
      std::span<const uv::Buffer> data, bool fin,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    Send(kOpCont | (fin ? kFlagFin : 0), data, std::move(callback));
  }

  /**
   * Send a continuation frame.  This is used to send additional parts of a
   * message started with SendTextFragment() or SendBinaryFragment().
   * @param data Data to send
   * @param fin Set to true if this is the final fragment of the message
   * @param callback Callback which is invoked when the write completes.
   */
  void SendFragment(
      std::initializer_list<uv::Buffer> data, bool fin,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    SendFragment({data.begin(), data.end()}, fin, std::move(callback));
  }

  /**
   * Send a ping frame with no data.
   * @param callback Optional callback which is invoked when the ping frame
   *                 write completes.
   */
  void SendPing(std::function<void(uv::Error)> callback = nullptr) {
    SendPing({}, [f = std::move(callback)](auto bufs, uv::Error err) {
      if (f) {
        f(err);
      }
    });
  }

  /**
   * Send a ping frame.
   * @param data Data to send in the ping frame
   * @param callback Callback which is invoked when the ping frame
   *                 write completes.
   */
  void SendPing(
      std::span<const uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    SendControl(kFlagFin | kOpPing, data, std::move(callback));
  }

  /**
   * Send a ping frame.
   * @param data Data to send in the ping frame
   * @param callback Callback which is invoked when the ping frame
   *                 write completes.
   */
  void SendPing(
      std::initializer_list<uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    SendPing({data.begin(), data.end()}, std::move(callback));
  }

  /**
   * Send a pong frame with no data.
   * @param callback Optional callback which is invoked when the pong frame
   *                 write completes.
   */
  void SendPong(std::function<void(uv::Error)> callback = nullptr) {
    SendPong({}, [f = std::move(callback)](auto bufs, uv::Error err) {
      if (f) {
        f(err);
      }
    });
  }

  /**
   * Send a pong frame.
   * @param data Data to send in the pong frame
   * @param callback Callback which is invoked when the pong frame
   *                 write completes.
   */
  void SendPong(
      std::span<const uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    SendControl(kFlagFin | kOpPong, data, std::move(callback));
  }

  /**
   * Send a pong frame.
   * @param data Data to send in the pong frame
   * @param callback Callback which is invoked when the pong frame
   *                 write completes.
   */
  void SendPong(
      std::initializer_list<uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    SendPong({data.begin(), data.end()}, std::move(callback));
  }

  /**
   * Send multiple frames.
   *
   * @param frames Frame type/data pairs
   * @param callback Callback which is invoked when the write completes.
   */
  void SendFrames(
      std::span<const Frame> frames,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback);

  /**
   * Try to send multiple frames. Tries to send as many frames as possible
   * immediately, and only queues the "last" frame it can (as the network queue
   * will almost always fill partway through a frame). The frames following
   * the last frame will NOT be queued for transmission; the caller is
   * responsible for how to handle (e.g. re-send) those frames (e.g. when the
   * callback is called).
   *
   * @param frames Frame type/data pairs
   * @param callback Callback which is invoked when the write completes of the
   *                 last frame that is not returned.
   * @return Remaining frames that will not be sent
   */
  std::span<const Frame> TrySendFrames(
      std::span<const Frame> frames,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback);

  /**
   * Returns whether or not a previous TrySendFrames is still in progress.
   * Calling TrySendFrames if this returns true will return all frames.
   *
   * @return True if a TryWrite is in progress
   */
  bool IsWriteInProgress() const { return m_writeInProgress; }

  /**
   * Fail the connection.
   */
  void Fail(uint16_t code = 1002, std::string_view reason = "protocol error");

  /**
   * Forcibly close the connection.
   */
  void Terminate(uint16_t code = 1006, std::string_view reason = "terminated");

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
   * Shuts down and closes the underlying stream.
   */
  void Shutdown();

  /**
   * Gets the last time data was received on the stream.
   * @return Timestamp
   */
  uint64_t GetLastReceivedTime() const { return m_lastReceivedTime; }

  /**
   * Open event.  Emitted when the connection is open and ready to communicate.
   * The parameter is the selected subprotocol.
   */
  sig::Signal<std::string_view> open;

  /**
   * Close event.  Emitted when the connection is closed.  The first parameter
   * is a numeric value indicating the status code explaining why the connection
   * has been closed.  The second parameter is a human-readable string
   * explaining the reason why the connection has been closed.
   */
  sig::Signal<uint16_t, std::string_view> closed;

  /**
   * Text message event.  Emitted when a text message is received.
   * The first parameter is the data, the second parameter is true if the
   * data is the last fragment of the message.
   */
  sig::Signal<std::string_view, bool> text;

  /**
   * Binary message event.  Emitted when a binary message is received.
   * The first parameter is the data, the second parameter is true if the
   * data is the last fragment of the message.
   */
  sig::Signal<std::span<const uint8_t>, bool> binary;

  /**
   * Ping event.  Emitted when a ping message is received.  A pong message is
   * automatically sent in response, so this is simply a notification.
   */
  sig::Signal<std::span<const uint8_t>> ping;

  /**
   * Pong event.  Emitted when a pong message is received.
   */
  sig::Signal<std::span<const uint8_t>> pong;

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

  // outgoing write request
  bool m_writeInProgress = false;
  class WriteReq;
  std::weak_ptr<WriteReq> m_curWriteReq;
  std::weak_ptr<WriteReq> m_lastWriteReq;

  // operating state
  State m_state = CONNECTING;

  // incoming message buffers/state
  uint64_t m_lastReceivedTime = 0;
  SmallVector<uint8_t, 14> m_header;
  size_t m_headerSize = 0;
  SmallVector<uint8_t, 1024> m_payload;
  SmallVector<uint8_t, 64> m_controlPayload;
  size_t m_frameStart = 0;
  uint64_t m_frameSize = UINT64_MAX;
  uint8_t m_fragmentOpcode = 0;

  // temporary data used only during client handshake
  class ClientHandshakeData;
  std::unique_ptr<ClientHandshakeData> m_clientHandshake;

  void StartClient(std::string_view uri, std::string_view host,
                   std::span<const std::string_view> protocols,
                   const ClientOptions& options);
  void StartServer(std::string_view key, std::string_view version,
                   std::string_view protocol);
  void SendClose(uint16_t code, std::string_view reason);
  void SetClosed(uint16_t code, std::string_view reason, bool failed = false);
  void HandleIncoming(uv::Buffer& buf, size_t size);
  void SendControl(
      uint8_t opcode, std::span<const uv::Buffer> data,
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback);
  void Send(uint8_t opcode, std::span<const uv::Buffer> data,
            std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
    SendFrames({{Frame{opcode, data}}}, std::move(callback));
  }
  void SendError(
      std::span<const Frame> frames,
      const std::function<void(std::span<uv::Buffer>, uv::Error)>& callback);
};

}  // namespace wpi

#endif  // WPINET_WEBSOCKET_H_
