/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_ASYNCFUNCTION_H_
#define WPIUTIL_WPI_UV_ASYNCFUNCTION_H_

#include <uv.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "wpi/STLExtras.h"
#include "wpi/condition_variable.h"
#include "wpi/mutex.h"
#include "wpi/uv/Handle.h"
#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

namespace detail {

class AsyncFunctionBase {
 public:
  virtual ~AsyncFunctionBase() {
    m_active = false;
    m_resultCv.notify_all();  // wake up any waiters
  }

 protected:
  wpi::mutex m_mutex;
  std::atomic_bool m_active{true};
  wpi::condition_variable m_resultCv;
};

template <typename R, typename... T>
struct AsyncFunctionHelper : public AsyncFunctionBase {
  inline void RunCall(const std::function<R(T...)>& func,
                      std::pair<std::thread::id, std::tuple<T...>>& v) {
    m_results.emplace_back(
        std::piecewise_construct, std::forward_as_tuple(v.first),
        std::forward_as_tuple(apply_tuple(func, std::move(v.second))));
  }

  inline R GetCallResult(std::thread::id from) {
    // wait for response
    std::unique_lock<wpi::mutex> lock(m_mutex);
    while (m_active) {
      // Did we get a response to *our* request?
      auto it = std::find_if(m_results.begin(), m_results.end(),
                             [=](const auto& r) { return r.first == from; });
      if (it != m_results.end()) {
        // Yes, remove it from the vector and we're done.
        auto rv = std::move(it->second);
        m_results.erase(it);
        return rv;
      }
      // No, keep waiting for a response
      m_resultCv.wait(lock);
    }

    return R();
  }

 private:
  std::vector<std::pair<std::thread::id, R>> m_results;
};

// void return value partial specialization
template <typename... T>
struct AsyncFunctionHelper<void, T...> : public AsyncFunctionBase {
  inline void RunCall(const std::function<void(T...)>& func,
                      std::pair<std::thread::id, std::tuple<T...>>& v) {
    apply_tuple(func, std::move(v.second));
    m_results.emplace_back(v.first);
  }

  inline void GetCallResult(std::thread::id from) {
    // wait for response
    std::unique_lock<wpi::mutex> lock(m_mutex);
    while (m_active) {
      // Did we get a response to *our* request?
      auto it = std::find(m_results.begin(), m_results.end(), from);
      if (it != m_results.end()) {
        // Yes, remove it from the vector and we're done.
        m_results.erase(it);
        return;
      }
      // No, keep waiting for a response
      m_resultCv.wait(lock);
    }
  }

 private:
  std::vector<std::thread::id> m_results;
};

}  // namespace detail

template <typename T>
class AsyncFunction;

/**
 * Function async handle.
 * Async handles allow the user to "wakeup" the event loop and have a function
 * called from another thread that returns a result to the calling thread.
 */
template <typename R, typename... T>
class AsyncFunction<R(T...)> final
    : public HandleImpl<AsyncFunction<R(T...)>, uv_async_t>,
      private detail::AsyncFunctionHelper<R, T...> {
  struct private_init {};

 public:
  AsyncFunction(std::function<R(T...)> func, const private_init&)
      : wakeup{func} {}
  ~AsyncFunction() noexcept override = default;

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   * @param func wakeup function to be called (sets wakeup value)
   */
  static std::shared_ptr<AsyncFunction> Create(
      Loop& loop, std::function<R(T...)> func = nullptr) {
    auto h = std::make_shared<AsyncFunction>(std::move(func), private_init{});
    int err = uv_async_init(loop.GetRaw(), h->GetRaw(), [](uv_async_t* handle) {
      auto& h = *static_cast<AsyncFunction*>(handle->data);
      std::lock_guard<wpi::mutex> lock(h.m_mutex);

      if (!h.m_params.empty()) {
        // for each set of parameters in the input queue
        for (auto&& v : h.m_params) {
          // call the wakeup function and put the result in the output queue
          // if the caller is waiting for it
          if (v.first == std::thread::id{})
            apply_tuple(h.wakeup, std::move(v.second));
          else
            h.RunCall(h.wakeup, v);
        }
        h.m_params.clear();
        // wake up any threads that might be waiting for the result
        h.m_resultCv.notify_all();
      }
    });
    if (err < 0) {
      loop.ReportError(err);
      return nullptr;
    }
    h->Keep();
    return h;
  }

  /**
   * Create an async handle.
   *
   * @param loop Loop object where this handle runs.
   * @param func wakeup function to be called (sets wakeup value)
   */
  static std::shared_ptr<AsyncFunction> Create(
      const std::shared_ptr<Loop>& loop,
      std::function<R(T...)> func = nullptr) {
    return Create(*loop, std::move(func));
  }

  /**
   * Wakeup the event loop, call the async function, and ignore any result.
   * This is non-blocking and does NOT wait until the async function returns.
   *
   * It’s safe to call this function from any thread EXCEPT the loop thread.
   * The async function will be called on the loop thread.
   */
  template <typename... U>
  void Send(U&&... u) {
    // add the parameters to the input queue
    {
      std::lock_guard<wpi::mutex> lock(this->m_mutex);
      m_params.emplace_back(std::piecewise_construct,
                            std::forward_as_tuple(std::thread::id{}),
                            std::forward_as_tuple(std::forward<U>(u)...));
    }

    // signal the loop
    this->Invoke(&uv_async_send, this->GetRaw());
  }

  /**
   * Wakeup the event loop, call the async function, and return the result.
   * This blocks the calling thread until the async function returns.
   *
   * It’s safe to call this function from any thread EXCEPT the loop thread.
   * The async function will be called on the loop thread.
   *
   * Returns a default-constructed result if this handle is destroyed while
   * waiting for a result.
   */
  template <typename... U>
  R Call(U&&... u) {
    std::thread::id from = std::this_thread::get_id();

    // add the parameters to the input queue
    {
      std::lock_guard<wpi::mutex> lock(this->m_mutex);
      m_params.emplace_back(std::piecewise_construct,
                            std::forward_as_tuple(from),
                            std::forward_as_tuple(std::forward<U>(u)...));
    }

    // signal the loop
    this->Invoke(&uv_async_send, this->GetRaw());

    // wait for response
    return this->GetCallResult(from);
  }

  /**
   * Function called (on event loop thread) when the async is called.
   */
  std::function<R(T...)> wakeup;

 private:
  std::vector<std::pair<std::thread::id, std::tuple<T...>>> m_params;
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_ASYNCFUNCTION_H_
