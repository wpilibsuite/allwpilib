/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_TCP_H_
#define WPIUTIL_WPI_UV_TCP_H_

#include <uv.h>

#include <chrono>
#include <functional>
#include <memory>

#include "wpi/Twine.h"
#include "wpi/uv/NetworkStream.h"

namespace wpi {
namespace uv {

class Loop;
class TcpConnectReq;

/**
 * TCP handle.
 * TCP handles are used to represent both TCP streams and servers.
 */
class Tcp final : public NetworkStreamImpl<Tcp, uv_tcp_t> {
  struct private_init {};

 public:
  using Time = std::chrono::duration<uint64_t, std::milli>;

  explicit Tcp(const private_init&) {}
  ~Tcp() noexcept override = default;

  /**
   * Create a TCP handle.
   *
   * @param loop Loop object where this handle runs.
   * @param flags Flags
   */
  static std::shared_ptr<Tcp> Create(Loop& loop,
                                     unsigned int flags = AF_UNSPEC);

  /**
   * Create a TCP handle.
   *
   * @param loop Loop object where this handle runs.
   * @param flags Flags
   */
  static std::shared_ptr<Tcp> Create(const std::shared_ptr<Loop>& loop,
                                     unsigned int flags = AF_UNSPEC) {
    return Create(*loop, flags);
  }

  /**
   * Reuse this handle.  This closes the handle, and after the close completes,
   * reinitializes it (identically to Create) and calls the provided callback.
   * Unlike Close(), it does NOT emit the closed signal, however, IsClosing()
   * will return true until the callback is called.  This does nothing if
   * IsClosing() is true (e.g. if Close() was called).
   *
   * @param flags Flags
   * @param callback Callback
   */
  void Reuse(std::function<void()> callback, unsigned int flags = AF_UNSPEC);

  /**
   * Accept incoming connection.
   *
   * This call is used in conjunction with `Listen()` to accept incoming
   * connections. Call this function after receiving a ListenEvent event to
   * accept the connection.
   * An error signal will be emitted in case of errors.
   *
   * When the connection signal is emitted it is guaranteed that this
   * function will complete successfully the first time. If you attempt to use
   * it more than once, it may fail.
   * It is suggested to only call this function once per connection signal.
   *
   * @return The stream handle for the accepted connection, or nullptr on error.
   */
  std::shared_ptr<Tcp> Accept();

  /**
   * Accept incoming connection.
   *
   * This call is used in conjunction with `Listen()` to accept incoming
   * connections. Call this function after receiving a connection signal to
   * accept the connection.
   * An error signal will be emitted in case of errors.
   *
   * When the connection signal is emitted it is guaranteed that this
   * function will complete successfully the first time. If you attempt to use
   * it more than once, it may fail.
   * It is suggested to only call this function once per connection signal.
   *
   * @param client Client stream object.
   * @return False on error.
   */
  bool Accept(const std::shared_ptr<Tcp>& client) {
    return NetworkStream::Accept(client);
  }

  /**
   * Open an existing file descriptor or SOCKET as a TCP handle.
   *
   * @note The passed file descriptor or SOCKET is not checked for its type, but
   * it's required that it represents a valid stream socket.
   *
   * @param sock A valid socket handle (either a file descriptor or a SOCKET).
   */
  void Open(uv_os_sock_t sock) { Invoke(&uv_tcp_open, GetRaw(), sock); }

  /**
   * Enable/Disable Nagle's algorithm.
   * @param enable True to enable it, false otherwise.
   * @return True in case of success, false otherwise.
   */
  bool SetNoDelay(bool enable) { return uv_tcp_nodelay(GetRaw(), enable) == 0; }

  /**
   * Enable/Disable TCP keep-alive.
   * @param enable True to enable it, false otherwise.
   * @param time Initial delay in seconds (use
   * `std::chrono::duration<unsigned int>`).
   * @return True in case of success, false otherwise.
   */
  bool SetKeepAlive(bool enable, Time time = Time{0}) {
    return uv_tcp_keepalive(GetRaw(), enable,
                            static_cast<unsigned>(time.count())) == 0;
  }

  /**
   * Enable/Disable simultaneous asynchronous accept requests.
   *
   * Enable/Disable simultaneous asynchronous accept requests that are
   * queued by the operating system when listening for new TCP
   * connections.
   * This setting is used to tune a TCP server for the desired performance.
   * Having simultaneous accepts can significantly improve the rate of
   * accepting connections (which is why it is enabled by default) but may
   * lead to uneven load distribution in multi-process setups.
   *
   * @param enable True to enable it, false otherwise.
   * @return True in case of success, false otherwise.
   */
  bool SetSimultaneousAccepts(bool enable) {
    return uv_tcp_simultaneous_accepts(GetRaw(), enable) == 0;
  }

  /**
   * Bind the handle to an IPv4 or IPv6 address and port.
   *
   * A successful call to this function does not guarantee that the call to
   * `Listen()` or `Connect()` will work properly.
   * An error signal can be emitted because of either this function or the
   * ones mentioned above.
   *
   * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
   * @param flags Optional additional flags.
   */
  void Bind(const sockaddr& addr, unsigned int flags = 0) {
    Invoke(&uv_tcp_bind, GetRaw(), &addr, flags);
  }

  void Bind(const sockaddr_in& addr, unsigned int flags = 0) {
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
  }

  void Bind(const sockaddr_in6& addr, unsigned int flags = 0) {
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
  }

  /**
   * Bind the handle to an IPv4 address and port.
   *
   * A successful call to this function does not guarantee that the call to
   * `Listen()` or `Connect()` will work properly.
   * An error signal can be emitted because of either this function or the
   * ones mentioned above.
   *
   * Available flags are:
   *
   * @param ip The address to which to bind.
   * @param port The port to which to bind.
   * @param flags Optional additional flags.
   */
  void Bind(const Twine& ip, unsigned int port, unsigned int flags = 0);

  /**
   * Bind the handle to an IPv6 address and port.
   *
   * A successful call to this function does not guarantee that the call to
   * `Listen()` or `Connect()` will work properly.
   * An error signal can be emitted because of either this function or the
   * ones mentioned above.
   *
   * Available flags are:
   *
   * @param ip The address to which to bind.
   * @param port The port to which to bind.
   * @param flags Optional additional flags.
   */
  void Bind6(const Twine& ip, unsigned int port, unsigned int flags = 0);

  /**
   * Get the current address to which the handle is bound.
   * @return The address (will be zeroed if an error occurred).
   */
  sockaddr_storage GetSock();

  /**
   * Get the address of the peer connected to the handle.
   * @return The address (will be zeroed if an error occurred).
   */
  sockaddr_storage GetPeer();

  /**
   * Establish an IPv4 or IPv6 TCP connection.
   *
   * On Windows if the addr is initialized to point to an unspecified address
   * (`0.0.0.0` or `::`) it will be changed to point to localhost. This is
   * done to match the behavior of Linux systems.
   *
   * The connected signal is emitted on the request when the connection has been
   * established.
   * The error signal is emitted on the request in case of errors during the
   * connection.
   *
   * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
   * @param req connection request
   */
  void Connect(const sockaddr& addr, const std::shared_ptr<TcpConnectReq>& req);

  void Connect(const sockaddr_in& addr,
               const std::shared_ptr<TcpConnectReq>& req) {
    Connect(reinterpret_cast<const sockaddr&>(addr), req);
  }

  void Connect(const sockaddr_in6& addr,
               const std::shared_ptr<TcpConnectReq>& req) {
    Connect(reinterpret_cast<const sockaddr&>(addr), req);
  }

  /**
   * Establish an IPv4 or IPv6 TCP connection.
   *
   * On Windows if the addr is initialized to point to an unspecified address
   * (`0.0.0.0` or `::`) it will be changed to point to localhost. This is
   * done to match the behavior of Linux systems.
   *
   * The callback is called when the connection has been established.  Errors
   * are reported to the stream error handler.
   *
   * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
   * @param callback Callback function to call when connection established
   */
  void Connect(const sockaddr& addr, std::function<void()> callback);

  void Connect(const sockaddr_in& addr, std::function<void()> callback) {
    Connect(reinterpret_cast<const sockaddr&>(addr), callback);
  }

  void Connect(const sockaddr_in6& addr, std::function<void()> callback) {
    Connect(reinterpret_cast<const sockaddr&>(addr), callback);
  }

  /**
   * Establish an IPv4 TCP connection.
   *
   * On Windows if the addr is initialized to point to an unspecified address
   * (`0.0.0.0` or `::`) it will be changed to point to localhost. This is
   * done to match the behavior of Linux systems.
   *
   * The connected signal is emitted on the request when the connection has been
   * established.
   * The error signal is emitted on the request in case of errors during the
   * connection.
   *
   * @param ip The address to which to connect to.
   * @param port The port to which to connect to.
   * @param req connection request
   */
  void Connect(const Twine& ip, unsigned int port,
               const std::shared_ptr<TcpConnectReq>& req);

  /**
   * Establish an IPv4 TCP connection.
   *
   * On Windows if the addr is initialized to point to an unspecified address
   * (`0.0.0.0` or `::`) it will be changed to point to localhost. This is
   * done to match the behavior of Linux systems.
   *
   * The callback is called when the connection has been established.  Errors
   * are reported to the stream error handler.
   *
   * @param ip The address to which to connect to.
   * @param port The port to which to connect to.
   * @param callback Callback function to call when connection established
   */
  void Connect(const Twine& ip, unsigned int port,
               std::function<void()> callback);

  /**
   * Establish an IPv6 TCP connection.
   *
   * On Windows if the addr is initialized to point to an unspecified address
   * (`0.0.0.0` or `::`) it will be changed to point to localhost. This is
   * done to match the behavior of Linux systems.
   *
   * The connected signal is emitted on the request when the connection has been
   * established.
   * The error signal is emitted on the request in case of errors during the
   * connection.
   *
   * @param ip The address to which to connect to.
   * @param port The port to which to connect to.
   * @param req connection request
   */
  void Connect6(const Twine& ip, unsigned int port,
                const std::shared_ptr<TcpConnectReq>& req);

  /**
   * Establish an IPv6 TCP connection.
   *
   * On Windows if the addr is initialized to point to an unspecified address
   * (`0.0.0.0` or `::`) it will be changed to point to localhost. This is
   * done to match the behavior of Linux systems.
   *
   * The callback is called when the connection has been established.  Errors
   * are reported to the stream error handler.
   *
   * @param ip The address to which to connect to.
   * @param port The port to which to connect to.
   * @param callback Callback function to call when connection established
   */
  void Connect6(const Twine& ip, unsigned int port,
                std::function<void()> callback);

 private:
  Tcp* DoAccept() override;

  struct ReuseData {
    std::function<void()> callback;
    unsigned int flags;
  };
  std::unique_ptr<ReuseData> m_reuseData;
};

/**
 * TCP connection request.
 */
class TcpConnectReq : public ConnectReq {
 public:
  Tcp& GetStream() const {
    return *static_cast<Tcp*>(&ConnectReq::GetStream());
  }
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_TCP_H_
