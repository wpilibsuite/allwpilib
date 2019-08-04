/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_HANDLE_H_
#define WPIUTIL_WPI_UV_HANDLE_H_

#include <uv.h>

#include <functional>
#include <memory>
#include <utility>

#include "wpi/Signal.h"
#include "wpi/StringRef.h"
#include "wpi/uv/Buffer.h"
#include "wpi/uv/Error.h"
#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

/**
 * Handle.
 * Handles are not moveable or copyable and cannot be directly constructed.
 * This class provides shared_ptr ownership and shared_from_this.
 * Use the specific handle type Create() functions to create handles.
 */
class Handle : public std::enable_shared_from_this<Handle> {
 public:
  using Type = uv_handle_type;

  Handle(const Handle&) = delete;
  Handle(Handle&&) = delete;
  Handle& operator=(const Handle&) = delete;
  Handle& operator=(Handle&&) = delete;
  virtual ~Handle() noexcept;

  /**
   * Get the type of the handle.
   *
   * A base handle offers no functionality to promote it to the actual handle
   * type. By means of this function, the type of the underlying handle as
   * specified by Type is made available.
   *
   * @return The actual type of the handle.
   */
  Type GetType() const noexcept { return m_uv_handle->type; }

  /**
   * Get the name of the type of the handle.  E.g. "pipe" for pipe handles.
   */
  StringRef GetTypeName() const noexcept {
    return uv_handle_type_name(m_uv_handle->type);
  }

  /**
   * Get the loop where this handle runs.
   *
   * @return The loop.
   */
  std::shared_ptr<Loop> GetLoop() const noexcept {
    return GetLoopRef().shared_from_this();
  }

  /**
   * Get the loop where this handle runs.
   *
   * @return The loop.
   */
  Loop& GetLoopRef() const noexcept {
    return *static_cast<Loop*>(m_uv_handle->loop->data);
  }

  /**
   * Check if the handle is active.
   *
   * What _active_ means depends on the type of handle:
   *
   * * An AsyncHandle handle is always active and cannot be deactivated,
   * except by closing it with uv_close().
   * * A PipeHandle, TcpHandle, UDPHandle, etc. handle - basically any handle
   * that deals with I/O - is active when it is doing something that involves
   * I/O, like reading, writing, connecting, accepting new connections, etc.
   * * A CheckHandle, IdleHandle, TimerHandle, etc. handle is active when it
   * has been started with a call to `Start()`.
   *
   * Rule of thumb: if a handle of type `FooHandle` has a `Start()` member
   * method, then it’s active from the moment that method is called. Likewise,
   * `Stop()` deactivates the handle again.
   *
   * @return True if the handle is active, false otherwise.
   */
  bool IsActive() const noexcept { return uv_is_active(m_uv_handle) != 0; }

  /**
   * Check if a handle is closing or closed.
   *
   * This function should only be used between the initialization of the
   * handle and the arrival of the close callback.
   *
   * @return True if the handle is closing or closed, false otherwise.
   */
  bool IsClosing() const noexcept {
    return m_closed || uv_is_closing(m_uv_handle) != 0;
  }

  /**
   * Request handle to be closed.
   *
   * This **must** be called on each handle before memory is released.
   * In-progress requests are cancelled and this can result in error() being
   * emitted.
   *
   * The handle will emit closed() when finished.
   */
  void Close() noexcept;

  /**
   * Set if the loop is closing.
   *
   * This is set during EventLoopRunner.Stop(), and can be used for other cases
   * to indicate the loop should be closing. For instance for a uv_walk loop can
   * use this to close existing handles.
   *
   * @param loopClosing true to set the loop currently in closing stages.
   */
  void SetLoopClosing(bool loopClosing) noexcept {
    m_loopClosing = loopClosing;
  }

  /**
   * Get the loop closing status.
   *
   * This can be used from closed() in order to tell if a closing loop is the
   * reason for the close, or another reason.
   *
   * @return true if the loop is closing, otherwise false.
   */
  bool IsLoopClosing() const noexcept { return m_loopClosing; }

  /**
   * Reference the given handle.
   *
   * References are idempotent, that is, if a handle is already referenced
   * calling this function again will have no effect.
   */
  void Reference() noexcept { uv_ref(m_uv_handle); }

  /**
   * Unreference the given handle.
   *
   * References are idempotent, that is, if a handle is not referenced calling
   * this function again will have no effect.
   */
  void Unreference() noexcept { uv_unref(m_uv_handle); }

  /**
   * Check if the given handle is referenced.
   * @return True if the handle is referenced, false otherwise.
   */
  bool HasReference() const noexcept { return uv_has_ref(m_uv_handle) != 0; }

  /**
   * Return the size of the underlying handle type.
   * @return The size of the underlying handle type.
   */
  size_t RawSize() const noexcept { return uv_handle_size(m_uv_handle->type); }

  /**
   * Get the underlying handle data structure.
   *
   * @return The underlying handle data structure.
   */
  uv_handle_t* GetRawHandle() const noexcept { return m_uv_handle; }

  /**
   * Set the functions used for allocating and releasing buffers.  The size
   * passed to the allocator function is a "suggested" size--it's just an
   * indication, not related in any way to the pending data to be read.  The
   * user is free to allocate the amount of memory they decide.  For example,
   * applications with custom allocation schemes may decide to use a different
   * size which matches the memory chunks they already have for other purposes.
   *
   * @warning Be very careful changing the allocator after the loop has started
   * running; there are no interlocks between this and buffers currently in
   * flight.
   *
   * @param alloc Allocation function
   * @param dealloc Deallocation function
   */
  void SetBufferAllocator(std::function<Buffer(size_t)> alloc,
                          std::function<void(Buffer&)> dealloc) {
    m_allocBuf = alloc;
    m_freeBuf = dealloc;
  }

  /**
   * Free a buffer.  Uses the function provided to SetBufFree() or
   * Buffer::Deallocate by default.
   *
   * @param buf The buffer
   */
  void FreeBuf(Buffer& buf) const noexcept { m_freeBuf(buf); }

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
   * Error signal
   */
  sig::Signal<Error> error;

  /**
   * Closed signal
   */
  sig::Signal<> closed;

  /**
   * Report an error.
   * @param err Error code
   */
  void ReportError(int err) const { error(Error(err)); }

 protected:
  explicit Handle(uv_handle_t* uv_handle) : m_uv_handle{uv_handle} {
    m_uv_handle->data = this;
  }

  void Keep() noexcept { m_self = shared_from_this(); }
  void Release() noexcept { m_self.reset(); }
  void ForceClosed() noexcept { m_closed = true; }

  static void AllocBuf(uv_handle_t* handle, size_t size, uv_buf_t* buf);
  static void DefaultFreeBuf(Buffer& buf);

  template <typename F, typename... Args>
  bool Invoke(F&& f, Args&&... args) const {
    auto err = std::forward<F>(f)(std::forward<Args>(args)...);
    if (err < 0) ReportError(err);
    return err == 0;
  }

 private:
  std::shared_ptr<Handle> m_self;
  uv_handle_t* m_uv_handle;
  bool m_closed = false;
  bool m_loopClosing = false;
  std::function<Buffer(size_t)> m_allocBuf{&Buffer::Allocate};
  std::function<void(Buffer&)> m_freeBuf{&DefaultFreeBuf};
  std::shared_ptr<void> m_data;
};

/**
 * Handle.
 */
template <typename T, typename U>
class HandleImpl : public Handle {
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
  HandleImpl() : Handle{reinterpret_cast<uv_handle_t*>(new U)} {}
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_HANDLE_H_
