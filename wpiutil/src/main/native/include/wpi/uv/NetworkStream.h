/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_NETWORKSTREAM_H_
#define WPIUTIL_WPI_UV_NETWORKSTREAM_H_

#include <uv.h>

#include <functional>
#include <memory>

#include "wpi/Signal.h"
#include "wpi/uv/Stream.h"

namespace wpi {
namespace uv {

class NetworkStream;

/**
 * Connection request.
 */
class ConnectReq : public RequestImpl<ConnectReq, uv_connect_t> {
 public:
  ConnectReq();

  NetworkStream& GetStream() const {
    return *static_cast<NetworkStream*>(GetRaw()->handle->data);
  }

  /**
   * Connection completed signal.
   */
  sig::Signal<> connected;
};

/**
 * Network stream handle.
 * This is an abstract type; there are two network stream implementations (Tcp
 * and Pipe).
 */
class NetworkStream : public Stream {
 public:
  static constexpr int kDefaultBacklog = 128;

  std::shared_ptr<NetworkStream> shared_from_this() {
    return std::static_pointer_cast<NetworkStream>(Handle::shared_from_this());
  }

  std::shared_ptr<const NetworkStream> shared_from_this() const {
    return std::static_pointer_cast<const NetworkStream>(
        Handle::shared_from_this());
  }

  /**
   * Start listening for incoming connections.  When a new incoming connection
   * is received the connection signal is generated.
   * @param backlog the number of connections the kernel might queue, same as
   *        listen(2).
   */
  void Listen(int backlog = kDefaultBacklog);

  /**
   * Start listening for incoming connections.  This is a convenience wrapper
   * around `Listen(int)` that also connects a callback to the connection
   * signal.  When a new incoming connection is received the connection signal
   * is generated (and the callback is called).
   * @param callback the callback to call when a connection is received.
   *        `Accept()` should be called from this callback.
   * @param backlog the number of connections the kernel might queue, same as
   *        listen(2).
   */
  void Listen(std::function<void()> callback, int backlog = kDefaultBacklog);

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
  std::shared_ptr<NetworkStream> Accept() {
    return DoAccept()->shared_from_this();
  }

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
  bool Accept(const std::shared_ptr<NetworkStream>& client) {
    return Invoke(&uv_accept, GetRawStream(), client->GetRawStream());
  }

  /**
   * Signal generated when an incoming connection is received.
   */
  sig::Signal<> connection;

 protected:
  explicit NetworkStream(uv_stream_t* uv_stream) : Stream{uv_stream} {}

  virtual NetworkStream* DoAccept() = 0;
};

template <typename T, typename U>
class NetworkStreamImpl : public NetworkStream {
 public:
  std::shared_ptr<T> shared_from_this() {
    return std::static_pointer_cast<T>(Handle::shared_from_this());
  }

  std::shared_ptr<const T> shared_from_this() const {
    return std::static_pointer_cast<const T>(Handle::shared_from_this());
  }

  /**
   * Get the underlying handle data structure.
   *
   * @return The underlying handle data structure.
   */
  U* GetRaw() const noexcept {
    return reinterpret_cast<U*>(this->GetRawHandle());
  }

 protected:
  NetworkStreamImpl() : NetworkStream{reinterpret_cast<uv_stream_t*>(new U)} {}
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_NETWORKSTREAM_H_
