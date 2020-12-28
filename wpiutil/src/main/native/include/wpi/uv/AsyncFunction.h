// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_UV_ASYNCFUNCTION_H_
#define WPIUTIL_WPI_UV_ASYNCFUNCTION_H_

#include <stdint.h>
#include <uv.h>

#include <functional>
#include <memory>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "wpi/future.h"
#include "wpi/mutex.h"
#include "wpi/uv/Handle.h"
#include "wpi/uv/Loop.h"

namespace wpi::uv {

template <typename T>
class AsyncFunction;

/**
 * Function async handle.
 * Async handles allow the user to "wakeup" the event loop and have a function
 * called from another thread that returns a result to the calling thread.
 */
template <typename R, typename... T>
class AsyncFunction<R(T...)> final
    : public HandleImpl<AsyncFunction<R(T...)>, uv_async_t> {
  struct private_init {};

 public:
  AsyncFunction(const std::shared_ptr<Loop>& loop,
                std::function<void(promise<R>, T...)> func, const private_init&)
      : wakeup{func}, m_loop{loop} {}
  ~AsyncFunction() noexcept override {
    if (auto loop = m_loop.lock()) {
      this->Close();
    } else {
      this->ForceClosed();
    }
  }

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   * @param func wakeup function to be called (sets wakeup value); the function
   *             needs to return void, and its first parameter is the promise
   *             for the result.  If no value is set on the promise by the
   *             wakeup function, a default-constructed value is "returned".
   */
  static std::shared_ptr<AsyncFunction> Create(
      Loop& loop, std::function<void(promise<R>, T...)> func = nullptr) {
    return Create(loop.shared_from_this(), std::move(func));
  }

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   * @param func wakeup function to be called (sets wakeup value); the function
   *             needs to return void, and its first parameter is the promise
   *             for the result.  If no value is set on the promise by the
   *             wakeup function, a default-constructed value is "returned".
   */
  static std::shared_ptr<AsyncFunction> Create(
      const std::shared_ptr<Loop>& loop,
      std::function<void(promise<R>, T...)> func = nullptr) {
    auto h =
        std::make_shared<AsyncFunction>(loop, std::move(func), private_init{});
    int err =
        uv_async_init(loop->GetRaw(), h->GetRaw(), [](uv_async_t* handle) {
          auto& h = *static_cast<AsyncFunction*>(handle->data);
          std::unique_lock lock(h.m_mutex);

          if (!h.m_params.empty()) {
            // for each set of parameters in the input queue, call the wakeup
            // function and put the result in the output queue if the caller is
            // waiting for it
            for (auto&& v : h.m_params) {
              auto p = h.m_promises.CreatePromise(v.first);
              if (h.wakeup) {
                std::apply(h.wakeup,
                           std::tuple_cat(std::make_tuple(std::move(p)),
                                          std::move(v.second)));
              }
            }
            h.m_params.clear();
            // wake up any threads that might be waiting for the result
            lock.unlock();
            h.m_promises.Notify();
          }
        });
    if (err < 0) {
      loop->ReportError(err);
      return nullptr;
    }
    h->Keep();
    return h;
  }

  /**
   * Wakeup the event loop, call the async function, and return a future for
   * the result.
   *
   * It’s safe to call this function from any thread including the loop thread.
   * The async function will be called on the loop thread.
   *
   * The future will return a default-constructed result if this handle is
   * destroyed while waiting for a result.
   */
  template <typename... U>
  future<R> Call(U&&... u) {
    // create the future
    uint64_t req = m_promises.CreateRequest();

    auto loop = m_loop.lock();
    if (loop && loop->GetThreadId() == std::this_thread::get_id()) {
      // called from within the loop, just call the function directly
      wakeup(m_promises.CreatePromise(req), std::forward<U>(u)...);
      return m_promises.CreateFuture(req);
    }

    // add the parameters to the input queue
    {
      std::scoped_lock lock(m_mutex);
      m_params.emplace_back(std::piecewise_construct,
                            std::forward_as_tuple(req),
                            std::forward_as_tuple(std::forward<U>(u)...));
    }

    // signal the loop
    if (loop) {
      this->Invoke(&uv_async_send, this->GetRaw());
    }

    // return future
    return m_promises.CreateFuture(req);
  }

  template <typename... U>
  future<R> operator()(U&&... u) {
    return Call(std::forward<U>(u)...);
  }

  /**
   * Function called (on event loop thread) when the async is called.
   */
  std::function<void(promise<R>, T...)> wakeup;

 private:
  wpi::mutex m_mutex;
  std::vector<std::pair<uint64_t, std::tuple<T...>>> m_params;
  PromiseFactory<R> m_promises;
  std::weak_ptr<Loop> m_loop;
};

}  // namespace wpi::uv

#endif  // WPIUTIL_WPI_UV_ASYNCFUNCTION_H_
