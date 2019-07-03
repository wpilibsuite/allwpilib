/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_PIPE_H_
#define WPIUTIL_WPI_UV_PIPE_H_

#include <uv.h>

#include <functional>
#include <memory>
#include <string>

#include "wpi/Twine.h"
#include "wpi/uv/NetworkStream.h"

namespace wpi {
namespace uv {

class Loop;
class PipeConnectReq;

/**
 * Pipe handle.
 * Pipe handles provide an abstraction over local domain sockets on Unix and
 * named pipes on Windows.
 */
class Pipe final : public NetworkStreamImpl<Pipe, uv_pipe_t> {
  struct private_init {};

 public:
  explicit Pipe(const private_init&) {}
  ~Pipe() noexcept override = default;

  /**
   * Create a pipe handle.
   *
   * @param loop Loop object where this handle runs.
   * @param ipc Indicates if this pipe will be used for handle passing between
   *            processes.
   */
  static std::shared_ptr<Pipe> Create(Loop& loop, bool ipc = false);

  /**
   * Create a pipe handle.
   *
   * @param loop Loop object where this handle runs.
   * @param ipc Indicates if this pipe will be used for handle passing between
   *            processes.
   */
  static std::shared_ptr<Pipe> Create(const std::shared_ptr<Loop>& loop,
                                      bool ipc = false) {
    return Create(*loop, ipc);
  }

  /**
   * Reuse this handle.  This closes the handle, and after the close completes,
   * reinitializes it (identically to Create) and calls the provided callback.
   * Unlike Close(), it does NOT emit the closed signal, however, IsClosing()
   * will return true until the callback is called.  This does nothing if
   * IsClosing() is true (e.g. if Close() was called).
   *
   * @param ipc IPC
   * @param callback Callback
   */
  void Reuse(std::function<void()> callback, bool ipc = false);

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
  std::shared_ptr<Pipe> Accept();

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
  bool Accept(const std::shared_ptr<Pipe>& client) {
    return NetworkStream::Accept(client);
  }

  /**
   * Open an existing file descriptor or HANDLE as a pipe.
   *
   * @note The passed file descriptor or HANDLE is not checked for its type, but
   * it's required that it represents a valid pipe.
   *
   * @param file A valid file handle (either a file descriptor or a HANDLE).
   */
  void Open(uv_file file) { Invoke(&uv_pipe_open, GetRaw(), file); }

  /**
   * Bind the pipe to a file path (Unix) or a name (Windows).
   *
   * @note Paths on Unix get truncated to `sizeof(sockaddr_un.sun_path)` bytes,
   * typically between 92 and 108 bytes.
   *
   * @param name File path (Unix) or name (Windows).
   */
  void Bind(const Twine& name);

  /**
   * Connect to the Unix domain socket or the named pipe.
   *
   * @note Paths on Unix get truncated to `sizeof(sockaddr_un.sun_path)` bytes,
   * typically between 92 and 108 bytes.
   *
   * HandleConnected() is called on the request when the connection has been
   * established.
   * HandleError() is called on the request in case of errors during the
   * connection.
   *
   * @param name File path (Unix) or name (Windows).
   * @param req connection request
   */
  void Connect(const Twine& name, const std::shared_ptr<PipeConnectReq>& req);

  /**
   * Connect to the Unix domain socket or the named pipe.
   *
   * @note Paths on Unix get truncated to `sizeof(sockaddr_un.sun_path)` bytes,
   * typically between 92 and 108 bytes.
   *
   * The callback is called when the connection has been established.  Errors
   * are reported to the stream error handler.
   *
   * @param name File path (Unix) or name (Windows).
   * @param callback Callback function to call when connection established
   */
  void Connect(const Twine& name, std::function<void()> callback);

  /**
   * Get the name of the Unix domain socket or the named pipe.
   * @return The name (will be empty if an error occurred).
   */
  std::string GetSock();

  /**
   * Get the name of the Unix domain socket or the named pipe to which the
   * handle is connected.
   * @return The name (will be empty if an error occurred).
   */
  std::string GetPeer();

  /**
   * Set the number of pending pipe instance handles when the pipe server is
   * waiting for connections.
   * @note This setting applies to Windows only.
   * @param count Number of pending handles.
   */
  void SetPendingInstances(int count) {
    uv_pipe_pending_instances(GetRaw(), count);
  }

  /**
   * Alters pipe permissions, allowing it to be accessed from processes run
   * by different users.  Makes the pipe writable or readable by all users.
   * Mode can be UV_WRITABLE, UV_READABLE, or both.  This function is blocking.
   * @param flags chmod flags
   */
  void Chmod(int flags) { Invoke(&uv_pipe_chmod, GetRaw(), flags); }

 private:
  Pipe* DoAccept() override;

  struct ReuseData {
    std::function<void()> callback;
    bool ipc;
  };
  std::unique_ptr<ReuseData> m_reuseData;
};

/**
 * Pipe connection request.
 */
class PipeConnectReq : public ConnectReq {
 public:
  Pipe& GetStream() const {
    return *static_cast<Pipe*>(&ConnectReq::GetStream());
  }
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_PIPE_H_
