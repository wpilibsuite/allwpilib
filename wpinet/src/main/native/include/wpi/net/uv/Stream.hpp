// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_UV_STREAM_H_
#define WPINET_UV_STREAM_H_

#include <uv.h>

#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <memory>
#include <span>
#include <utility>

#include <wpi/Signal.h>

#include "wpinet/uv/Buffer.h"
#include "wpinet/uv/Handle.h"
#include "wpinet/uv/Request.h"

namespace wpi::uv {

class Stream;

/**
 * Shutdown request.
 */
class ShutdownReq : public RequestImpl<ShutdownReq, uv_shutdown_t> {
 public:
  ShutdownReq();

  Stream& GetStream() const {
    return *static_cast<Stream*>(GetRaw()->handle->data);
  }

  /**
   * Shutdown completed signal.
   */
  sig::Signal<> complete;
};

/**
 * Write request.
 */
class WriteReq : public RequestImpl<WriteReq, uv_write_t> {
 public:
  WriteReq();

  Stream& GetStream() const {
    return *static_cast<Stream*>(GetRaw()->handle->data);
  }

  /**
   * Write completed signal.  This is called even if an error occurred.
   * @param err error value
   */
  sig::Signal<Error> finish;
};

/**
 * Stream handle.
 * Stream handles provide an abstraction of a duplex communication channel.
 * This is an abstract type; there are three stream implementations (Tcp,
 * Pipe, and Tty).
 */
class Stream : public Handle {
 public:
  std::shared_ptr<Stream> shared_from_this() {
    return std::static_pointer_cast<Stream>(Handle::shared_from_this());
  }

  std::shared_ptr<const Stream> shared_from_this() const {
    return std::static_pointer_cast<const Stream>(Handle::shared_from_this());
  }

  /**
   * Shutdown the outgoing (write) side of a duplex stream. It waits for pending
   * write requests to complete.  HandleShutdownComplete() is called on the
   * request after shutdown is complete.
   *
   * @param req shutdown request
   */
  void Shutdown(const std::shared_ptr<ShutdownReq>& req);

  /**
   * Shutdown the outgoing (write) side of a duplex stream. It waits for pending
   * write requests to complete.  The callback is called after shutdown is
   * complete.  Errors will be reported to the stream error handler.
   *
   * @param callback Callback function to call when shutdown completes
   */
  void Shutdown(std::function<void()> callback = nullptr);

  /**
   * Start reading data from an incoming stream.
   *
   * This will only succeed after a connection has been established.
   *
   * A data signal will be emitted several times until there is no more
   * data to read or `StopRead()` is called.
   * An end signal will be emitted when there is no more data to read.
   */
  void StartRead();

  /**
   * Stop reading data from the stream.
   *
   * This function is idempotent and may be safely called on a stopped stream.
   */
  void StopRead() { Invoke(&uv_read_stop, GetRawStream()); }

  /**
   * Write data to the stream.
   *
   * Data are written in order. The lifetime of the data pointers passed in
   * the `bufs` parameter must exceed the lifetime of the write request.
   * An easy way to ensure this is to have the write request keep track of
   * the data and use either its Complete() function or destructor to free the
   * data.
   *
   * The finish signal will be emitted on the request object when the data
   * has been written (or if an error occurs).
   * The error signal will be emitted on the request object in case of errors.
   *
   * @param bufs The buffers to be written to the stream.
   * @param req write request
   */
  void Write(std::span<const Buffer> bufs,
             const std::shared_ptr<WriteReq>& req);

  /**
   * Write data to the stream.
   *
   * Data are written in order. The lifetime of the data pointers passed in
   * the `bufs` parameter must exceed the lifetime of the write request.
   * An easy way to ensure this is to have the write request keep track of
   * the data and use either its Complete() function or destructor to free the
   * data.
   *
   * The finish signal will be emitted on the request object when the data
   * has been written (or if an error occurs).
   * The error signal will be emitted on the request object in case of errors.
   *
   * @param bufs The buffers to be written to the stream.
   * @param req write request
   */
  void Write(std::initializer_list<Buffer> bufs,
             const std::shared_ptr<WriteReq>& req) {
    Write({bufs.begin(), bufs.end()}, req);
  }

  /**
   * Write data to the stream.
   *
   * Data are written in order. The lifetime of the data pointers passed in
   * the `bufs` parameter must exceed the lifetime of the write request.
   * The callback can be used to free data after the request completes.
   *
   * The callback will be called when the data has been written (even if an
   * error occurred).  Errors will be reported to the stream error handler.
   *
   * @param bufs The buffers to be written to the stream.
   * @param callback Callback function to call when the write completes
   */
  void Write(std::span<const Buffer> bufs,
             std::function<void(std::span<Buffer>, Error)> callback);

  /**
   * Write data to the stream.
   *
   * Data are written in order. The lifetime of the data pointers passed in
   * the `bufs` parameter must exceed the lifetime of the write request.
   * The callback can be used to free data after the request completes.
   *
   * The callback will be called when the data has been written (even if an
   * error occurred).  Errors will be reported to the stream error handler.
   *
   * @param bufs The buffers to be written to the stream.
   * @param callback Callback function to call when the write completes
   */
  void Write(std::initializer_list<Buffer> bufs,
             std::function<void(std::span<Buffer>, Error)> callback) {
    Write({bufs.begin(), bufs.end()}, std::move(callback));
  }

  /**
   * Queue a write request if it can be completed immediately.
   *
   * Same as `Write()`, but won’t queue a write request if it can’t be
   * completed immediately.
   * An error signal will be emitted in case of errors.
   *
   * @param bufs The buffers to be written to the stream.
   * @return Number of bytes written, or negative (error code) on error
   */
  [[nodiscard]]
  int TryWrite(std::span<const Buffer> bufs);

  /**
   * Queue a write request if it can be completed immediately.
   *
   * Same as `Write()`, but won’t queue a write request if it can’t be
   * completed immediately.
   * An error signal will be emitted in case of errors.
   *
   * @param bufs The buffers to be written to the stream.
   * @return Number of bytes written, or negative (error code) on error
   */
  [[nodiscard]]
  int TryWrite(std::initializer_list<Buffer> bufs) {
    return TryWrite({bufs.begin(), bufs.end()});
  }

  /**
   * Same as TryWrite() and extended write function for sending handles over a
   * pipe.
   *
   * Try to send a handle is not supported on Windows, where it returns
   * UV_EAGAIN.
   *
   * @param bufs The buffers to be written to the stream.
   * @param send send stream
   * @return Number of bytes written, or negative (error code) on error
   */
  [[nodiscard]]
  int TryWrite2(std::span<const Buffer> bufs, Stream& send);

  /**
   * Same as TryWrite() and extended write function for sending handles over a
   * pipe.
   *
   * Try to send a handle is not supported on Windows, where it returns
   * UV_EAGAIN.
   *
   * @param bufs The buffers to be written to the stream.
   * @param send send stream
   * @return Number of bytes written, or negative (error code) on error
   */
  [[nodiscard]]
  int TryWrite2(std::initializer_list<Buffer> bufs, Stream& send) {
    return TryWrite2({bufs.begin(), bufs.end()}, send);
  }

  /**
   * Check if the stream is readable.
   * @return True if the stream is readable, false otherwise.
   */
  bool IsReadable() const noexcept {
    return uv_is_readable(GetRawStream()) == 1;
  }

  /**
   * @brief Checks if the stream is writable.
   * @return True if the stream is writable, false otherwise.
   */
  bool IsWritable() const noexcept {
    return uv_is_writable(GetRawStream()) == 1;
  }

  /**
   * Enable or disable blocking mode for a stream.
   *
   * When blocking mode is enabled all writes complete synchronously. The
   * interface remains unchanged otherwise, e.g. completion or failure of the
   * operation will still be reported through events which are emitted
   * asynchronously.
   *
   * @param enable True to enable blocking mode, false otherwise.
   * @return True in case of success, false otherwise.
   */
  bool SetBlocking(bool enable) noexcept {
    return uv_stream_set_blocking(GetRawStream(), enable) == 0;
  }

  /**
   * Gets the amount of queued bytes waiting to be sent.
   * @return Amount of queued bytes waiting to be sent.
   */
  size_t GetWriteQueueSize() const noexcept {
    return GetRawStream()->write_queue_size;
  }

  /**
   * Get the underlying stream data structure.
   *
   * @return The underlying stream data structure.
   */
  uv_stream_t* GetRawStream() const noexcept {
    return reinterpret_cast<uv_stream_t*>(GetRawHandle());
  }

  /**
   * Signal generated when data was read on a stream.
   */
  sig::Signal<Buffer&, size_t> data;

  /**
   * Signal generated when no more read data is available.
   */
  sig::Signal<> end;

 protected:
  explicit Stream(uv_stream_t* uv_stream)
      : Handle{reinterpret_cast<uv_handle_t*>(uv_stream)} {}
};

template <typename T, typename U>
class StreamImpl : public Stream {
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
  StreamImpl() : Stream{static_cast<uv_stream_t*>(std::malloc(sizeof(U)))} {}
};

}  // namespace wpi::uv

#endif  // WPINET_UV_STREAM_H_
