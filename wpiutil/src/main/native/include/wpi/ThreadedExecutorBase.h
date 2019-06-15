/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_THREADEDEXECUTORBASE_H_
#define WPIUTIL_WPI_THREADEDEXECUTORBASE_H_

#include <stdint.h>

#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "wpi/future.h"
#include "wpi/mutex.h"

namespace wpi {

template <typename Derived, typename Handle>
class ThreadedExecutorBase;

/**
 * Executor handle base class to use as a basis for threaded executors.
 */
class ThreadedExecutorBaseHandle {
  template <typename D, typename H>
  friend class ThreadedExecutorBase;

 protected:
  // type-erase the PromiseFactory via lambda capture, but put the req in the
  // vector to reduce the capture size enough to avoid a memory allocation in
  // std::function
  struct Entry {
    template <typename F>
    Entry(void* factory_, uint64_t req_, F&& func_)
        : factory(factory_), req(req_), func(std::forward<F>(func_)) {}
    void* factory;
    uint64_t req;
    std::function<void(void*, uint64_t)> func;
  };
  std::vector<Entry> m_queue;
};

/**
 * Executor base class to use as a basis for threaded executors.
 * Uses CTRP.
 * Handle class must define a Signal() function to signal the thread and a
 * GetThreadId() function to get the thread id (not the current thread id),
 * and must also provide a mutex named m_mutex.
 */
template <typename Derived, typename Handle>
class ThreadedExecutorBase {
 public:
  ThreadedExecutorBase() = default;
  explicit ThreadedExecutorBase(std::shared_ptr<Handle> handle)
      : m_handle(std::move(handle)) {}

  template <typename F>
  void execute(F&& func) const;

  template <typename F,
            typename R = typename std::result_of_t<std::decay_t<F>()>>
  future<R> twoway_execute(F&& func) const;

  template <typename R, typename F, typename Future>
  continuable_future<R, Derived> then_execute(F&& func, Future&& pred) const;

 private:
  std::shared_ptr<Handle> m_handle;
};

template <typename Derived, typename Handle>
template <typename F>
void ThreadedExecutorBase<Derived, Handle>::execute(F&& func) const {
  if (!m_handle) return;
  if (m_handle->GetThreadId() == std::this_thread::get_id()) {
    // called from within the loop, just call the function directly
    func();
    return;
  }

  // add to the input queue
  {
    std::scoped_lock lock(m_handle->m_mutex);
    m_handle->m_queue.emplace_back(
        nullptr, 0, [fun = std::forward<F>(func)](void*, uint64_t) { fun(); });
  }

  m_handle->Signal();
}

template <typename Derived, typename Handle>
template <typename F, typename R>
future<R> ThreadedExecutorBase<Derived, Handle>::twoway_execute(
    F&& func) const {
  if (!m_handle) {
    if constexpr (std::is_void_v<R>) {
      return make_ready_future();
    } else {
      return make_ready_future<R>(R());
    }
  }
  if (m_handle->GetThreadId() == std::this_thread::get_id()) {
    // called from within the loop, just call the function directly
    if constexpr (std::is_void_v<R>) {
      func();
      return make_ready_future();
    } else {
      return make_ready_future(func());
    }
  }

  // create the future
  using Factory = PromiseFactory<R>;
  auto& factory = Factory::GetInstance();
  uint64_t req = factory.CreateRequest();

  // add to the input queue
  {
    std::scoped_lock lock(m_handle->m_mutex);
    if constexpr (std::is_void_v<R>) {
      m_handle->m_queue.emplace_back(&factory, req,
                                     [func](void* f, uint64_t r) {
                                       func();
                                       static_cast<Factory*>(f)->SetValue(r);
                                     });
    } else {
      m_handle->m_queue.emplace_back(
          &factory, req, [func](void* f, uint64_t r) {
            static_cast<Factory*>(f)->SetValue(r, func());
          });
    }
  }

  m_handle->Signal();

  // return future
  return factory.CreateFuture(req);
}

template <typename Derived, typename Handle>
template <typename R, typename F, typename Future>
continuable_future<R, Derived>
ThreadedExecutorBase<Derived, Handle>::then_execute(F&& func,
                                                    Future&& pred) const {
  // create the future
  auto [predFactory, predRequest] = std::move(pred).ToFactory();
  using Factory = PromiseFactory<R>;
  auto& factory = Factory::GetInstance();

  uint64_t req = factory.CreateRequest();
  if constexpr (std::is_void_v<typename Future::value_type>) {
    if constexpr (std::is_void_v<R>) {
      predFactory->SetThen(
          predRequest, &factory, req, m_handle,
          [func](void* f, uint64_t r, std::shared_ptr<void> e) {
            auto handle = static_cast<Handle*>(e.get());
            if (handle->GetThreadId() == std::this_thread::get_id()) {
              // called from within the loop, just call the function directly
              func();
              static_cast<Factory*>(f)->SetValue(r);
              return;
            }
            // add to the input queue
            std::scoped_lock lock(handle->m_mutex);
            handle->m_queue.emplace_back(f, r, [func](void* f2, uint64_t r2) {
              func();
              static_cast<Factory*>(f2)->SetValue(r2);
            });
            handle->Signal();
          });
    } else {
      predFactory->SetThen(
          predRequest, &factory, req, m_handle,
          [func](void* f, uint64_t r, std::shared_ptr<void> e) {
            auto handle = static_cast<Handle*>(e.get());
            if (handle->GetThreadId() == std::this_thread::get_id()) {
              // called from within the loop, just call the function directly
              static_cast<Factory*>(f)->SetValue(r, func());
              return;
            }
            // add to the input queue
            std::scoped_lock lock(handle->m_mutex);
            handle->m_queue.emplace_back(f, r, [func](void* f2, uint64_t r2) {
              static_cast<Factory*>(f2)->SetValue(r2, func());
            });
            handle->Signal();
          });
    }
  } else {
    if constexpr (std::is_void_v<R>) {
      predFactory->SetThen(
          predRequest, &factory, req, m_handle,
          [func](void* f, uint64_t r, std::shared_ptr<void> e,
                 typename Future::value_type value) {
            auto handle = static_cast<Handle*>(e.get());
            if (handle->GetThreadId() == std::this_thread::get_id()) {
              // called from within the loop, just call the function directly
              func(std::move(value));
              static_cast<Factory*>(f)->SetValue(r);
              return;
            }
            // add to the input queue
            std::scoped_lock lock(handle->m_mutex);
            handle->m_queue.emplace_back(
                f, r, [func, v = std::move(value)](void* f2, uint64_t r2) {
                  func(std::move(v));
                  static_cast<Factory*>(f2)->SetValue(r2);
                });
            handle->Signal();
          });
    } else {
      predFactory->SetThen(
          predRequest, &factory, req, m_handle,
          [func](void* f, uint64_t r, std::shared_ptr<void> e,
                 typename Future::value_type value) {
            auto handle = static_cast<Handle*>(e.get());
            if (handle->GetThreadId() == std::this_thread::get_id()) {
              // called from within the loop, just call the function directly
              static_cast<Factory*>(f)->SetValue(r, func(std::move(value)));
              return;
            }
            // add to the input queue
            std::scoped_lock lock(handle->m_mutex);
            handle->m_queue.emplace_back(
                f, r, [func, v = std::move(value)](void* f2, uint64_t r2) {
                  static_cast<Factory*>(f2)->SetValue(r2, func(std::move(v)));
                });
            handle->Signal();
          });
    }
  }

  return factory.CreateContinuableFuture(req,
                                         *static_cast<const Derived*>(this));
}

}  // namespace wpi

#endif  // WPIUTIL_WPI_THREADEDEXECUTORBASE_H_
