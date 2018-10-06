/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_ASYNC_H_
#define WPIUTIL_WPI_UV_ASYNC_H_

#include <uv.h>

#include <memory>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "wpi/STLExtras.h"
#include "wpi/Signal.h"
#include "wpi/mutex.h"
#include "wpi/uv/Handle.h"
#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

/**
 * Async handle.
 * Async handles allow the user to "wakeup" the event loop and have a signal
 * generated from another thread.
 *
 * Data may be passed into the callback called on the event loop by using
 * template parameters.  If data parameters are used, the async callback will
 * be called once for every call to Send().  If no data parameters are used,
 * the async callback may or may not be called for every call to Send() (e.g.
 * the calls may be coaleasced).
 */
template <typename... T>
class Async final : public HandleImpl<Async<T...>, uv_async_t> {
  struct private_init {};

 public:
  Async(const std::shared_ptr<Loop>& loop, const private_init&)
      : m_loop{loop} {}
  ~Async() noexcept override {
    if (auto loop = m_loop.lock())
      this->Close();
    else
      this->ForceClosed();
  }

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Async> Create(Loop& loop) {
    return Create(loop.shared_from_this());
  }

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Async> Create(const std::shared_ptr<Loop>& loop) {
    auto h = std::make_shared<Async>(loop, private_init{});
    int err =
        uv_async_init(loop->GetRaw(), h->GetRaw(), [](uv_async_t* handle) {
          auto& h = *static_cast<Async*>(handle->data);
          std::lock_guard<wpi::mutex> lock(h.m_mutex);
          for (auto&& v : h.m_data) apply_tuple(h.wakeup, v);
          h.m_data.clear();
        });
    if (err < 0) {
      loop->ReportError(err);
      return nullptr;
    }
    h->Keep();
    return h;
  }

  /**
   * Wakeup the event loop and emit the event.
   *
   * It’s safe to call this function from any thread including the loop thread.
   * An async event will be emitted on the loop thread.
   */
  template <typename... U>
  void Send(U&&... u) {
    auto loop = m_loop.lock();
    if (loop && loop->GetThreadId() == std::this_thread::get_id()) {
      // called from within the loop, just call the function directly
      wakeup(std::forward<U>(u)...);
      return;
    }

    {
      std::lock_guard<wpi::mutex> lock(m_mutex);
      m_data.emplace_back(std::forward_as_tuple(std::forward<U>(u)...));
    }
    if (loop) this->Invoke(&uv_async_send, this->GetRaw());
  }

  /**
   * Signal generated (on event loop thread) when the async event occurs.
   */
  sig::Signal<T...> wakeup;

 private:
  wpi::mutex m_mutex;
  std::vector<std::tuple<T...>> m_data;
  std::weak_ptr<Loop> m_loop;
};

/**
 * Async specialization for no data parameters.  The async callback may or may
 * not be called for every call to Send() (e.g. the calls may be coaleasced).
 */
template <>
class Async<> final : public HandleImpl<Async<>, uv_async_t> {
  struct private_init {};

 public:
  Async(const std::shared_ptr<Loop>& loop, const private_init&)
      : m_loop(loop) {}
  ~Async() noexcept override;

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Async> Create(Loop& loop) {
    return Create(loop.shared_from_this());
  }

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<Async> Create(const std::shared_ptr<Loop>& loop);

  /**
   * Wakeup the event loop and emit the event.
   *
   * It’s safe to call this function from any thread.
   * An async event will be emitted on the loop thread.
   */
  void Send() {
    if (auto loop = m_loop.lock()) Invoke(&uv_async_send, GetRaw());
  }

  /**
   * Signal generated (on event loop thread) when the async event occurs.
   */
  sig::Signal<> wakeup;

 private:
  std::weak_ptr<Loop> m_loop;
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_ASYNC_H_
