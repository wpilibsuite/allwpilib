/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_ASYNCEXECUTOR_H_
#define WPIUTIL_WPI_UV_ASYNCEXECUTOR_H_

#include <uv.h>

#include <memory>
#include <thread>
#include <utility>

#include "wpi/ThreadedExecutorBase.h"
#include "wpi/uv/Handle.h"
#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

class AsyncExecutor;

namespace detail {

/**
 * Async executor handle.
 * Async handles allow the user to "wakeup" the event loop and have a function
 * called from another thread that returns a result to the calling thread.
 */
class AsyncExecutorHandle final
    : public ThreadedExecutorBaseHandle,
      public HandleImpl<AsyncExecutorHandle, uv_async_t> {
  struct private_init {};
  friend class ::wpi::uv::AsyncExecutor;
  friend class ThreadedExecutorBase<AsyncExecutor, AsyncExecutorHandle>;

 public:
  AsyncExecutorHandle(const std::shared_ptr<Loop>& loop, const private_init&)
      : m_loop{loop} {}
  ~AsyncExecutorHandle() noexcept override;

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<AsyncExecutorHandle> Create(Loop& loop) {
    return Create(loop.shared_from_this());
  }

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<AsyncExecutorHandle> Create(
      const std::shared_ptr<Loop>& loop);

  AsyncExecutor GetExecutor();

 private:
  void Signal() {
    if (auto loop = m_loop.lock()) Invoke(&uv_async_send, GetRaw());
  }

  std::thread::id GetThreadId() {
    if (auto loop = m_loop.lock())
      return loop->GetThreadId();
    else
      return std::thread::id{};
  }

  wpi::mutex m_mutex;
  std::weak_ptr<Loop> m_loop;
};

}  // namespace detail

class AsyncExecutor final
    : public ThreadedExecutorBase<AsyncExecutor, detail::AsyncExecutorHandle> {
 public:
  AsyncExecutor() = default;
  explicit AsyncExecutor(std::shared_ptr<detail::AsyncExecutorHandle> handle)
      : ThreadedExecutorBase(std::move(handle)) {}
};

inline AsyncExecutor detail::AsyncExecutorHandle::GetExecutor() {
  return AsyncExecutor{shared_from_this()};
}

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_ASYNCEXECUTOR_H_
