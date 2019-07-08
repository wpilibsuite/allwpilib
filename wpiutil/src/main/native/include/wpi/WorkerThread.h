/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_WORKERTHREAD_H_
#define WPIUTIL_WPI_WORKERTHREAD_H_

#include <functional>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "wpi/SafeThread.h"
#include "wpi/future.h"
#include "wpi/uv/Async.h"

namespace wpi {

namespace detail {

template <typename R>
struct WorkerThreadAsync {
  using AfterWorkFunction = std::function<void(R)>;

  ~WorkerThreadAsync() { UnsetLoop(); }

  void SetLoop(uv::Loop& loop) {
    auto async = uv::Async<AfterWorkFunction, R>::Create(loop);
    async->wakeup.connect(
        [](AfterWorkFunction func, R result) { func(result); });
    m_async = async;
  }

  void UnsetLoop() {
    if (auto async = m_async.lock()) {
      async->Close();
      m_async.reset();
    }
  }

  std::weak_ptr<uv::Async<AfterWorkFunction, R>> m_async;
};

template <>
struct WorkerThreadAsync<void> {
  using AfterWorkFunction = std::function<void()>;

  ~WorkerThreadAsync() { RemoveLoop(); }

  void SetLoop(uv::Loop& loop) {
    auto async = uv::Async<AfterWorkFunction>::Create(loop);
    async->wakeup.connect([](AfterWorkFunction func) { func(); });
    m_async = async;
  }

  void RemoveLoop() {
    if (auto async = m_async.lock()) {
      async->Close();
      m_async.reset();
    }
  }

  std::weak_ptr<uv::Async<AfterWorkFunction>> m_async;
};

template <typename R, typename... T>
struct WorkerThreadRequest {
  using WorkFunction = std::function<R(T...)>;
  using AfterWorkFunction = typename WorkerThreadAsync<R>::AfterWorkFunction;

  WorkerThreadRequest() = default;
  WorkerThreadRequest(uint64_t promiseId_, WorkFunction work_,
                      std::tuple<T...> params_)
      : promiseId(promiseId_),
        work(std::move(work_)),
        params(std::move(params_)) {}
  WorkerThreadRequest(WorkFunction work_, AfterWorkFunction afterWork_,
                      std::tuple<T...> params_)
      : promiseId(0),
        work(std::move(work_)),
        afterWork(std::move(afterWork_)),
        params(std::move(params_)) {}

  uint64_t promiseId;
  WorkFunction work;
  AfterWorkFunction afterWork;
  std::tuple<T...> params;
};

template <typename R, typename... T>
class WorkerThreadThread : public SafeThread {
 public:
  using Request = WorkerThreadRequest<R, T...>;

  void Main() override;

  std::vector<Request> m_requests;
  PromiseFactory<R> m_promises;
  detail::WorkerThreadAsync<R> m_async;
};

template <typename R, typename... T>
void RunWorkerThreadRequest(WorkerThreadThread<R, T...>& thr,
                            WorkerThreadRequest<R, T...>& req) {
  R result = std::apply(req.work, std::move(req.params));
  if (req.afterWork) {
    if (auto async = thr.m_async.m_async.lock())
      async->Send(std::move(req.afterWork), std::move(result));
  } else {
    thr.m_promises.SetValue(req.promiseId, std::move(result));
  }
}

template <typename... T>
void RunWorkerThreadRequest(WorkerThreadThread<void, T...>& thr,
                            WorkerThreadRequest<void, T...>& req) {
  std::apply(req.work, req.params);
  if (req.afterWork) {
    if (auto async = thr.m_async.m_async.lock())
      async->Send(std::move(req.afterWork));
  } else {
    thr.m_promises.SetValue(req.promiseId);
  }
}

template <typename R, typename... T>
void WorkerThreadThread<R, T...>::Main() {
  std::vector<Request> requests;
  while (m_active) {
    std::unique_lock lock(m_mutex);
    m_cond.wait(lock, [&] { return !m_active || !m_requests.empty(); });
    if (!m_active) break;

    // don't want to hold the lock while executing the callbacks
    requests.swap(m_requests);
    lock.unlock();

    for (auto&& req : requests) {
      if (!m_active) break;  // requests may be long-running
      RunWorkerThreadRequest(*this, req);
    }
    requests.clear();
    m_promises.Notify();
  }
}

}  // namespace detail

template <typename T>
class WorkerThread;

template <typename R, typename... T>
class WorkerThread<R(T...)> final {
  using Thread = detail::WorkerThreadThread<R, T...>;

 public:
  using WorkFunction = std::function<R(T...)>;
  using AfterWorkFunction =
      typename detail::WorkerThreadAsync<R>::AfterWorkFunction;

  WorkerThread() { m_owner.Start(); }

  /**
   * Set the loop.  This must be called from the loop thread.
   * Subsequent calls to QueueWorkThen will run afterWork on the provided
   * loop (via an async handle).
   *
   * @param loop the loop to use for running afterWork routines
   */
  void SetLoop(uv::Loop& loop) {
    if (auto thr = m_owner.GetThread()) thr->m_async.SetLoop(loop);
  }

  /**
   * Set the loop.  This must be called from the loop thread.
   * Subsequent calls to QueueWorkThen will run afterWork on the provided
   * loop (via an async handle).
   *
   * @param loop the loop to use for running afterWork routines
   */
  void SetLoop(std::shared_ptr<uv::Loop> loop) { SetLoop(*loop); }

  /**
   * Unset the loop.  This must be called from the loop thread.
   * Subsequent calls to QueueWorkThen will no longer run afterWork.
   */
  void UnsetLoop() {
    if (auto thr = m_owner.GetThread()) thr->m_async.UnsetLoop();
  }

  /**
   * Get the handle used by QueueWorkThen() to run afterWork.
   * This handle is set by SetLoop().
   * Calling Close() on this handle is the same as calling UnsetLoop().
   *
   * @return The handle (if nullptr, no handle is set)
   */
  std::shared_ptr<uv::Handle> GetHandle() const {
    if (auto thr = m_owner.GetThread())
      return thr->m_async.m_async.lock();
    else
      return nullptr;
  }

  /**
   * Wakeup the worker thread, call the work function, and return a future for
   * the result.
   *
   * It’s safe to call this function from any thread.
   * The work function will be called on the worker thread.
   *
   * The future will return a default-constructed result if this class is
   * destroyed while waiting for a result.
   *
   * @param work Work function (called on worker thread)
   */
  template <typename... U>
  future<R> QueueWork(WorkFunction work, U&&... u) {
    if (auto thr = m_owner.GetThread()) {
      // create the future
      uint64_t req = thr->m_promises.CreateRequest();

      // add the parameters to the input queue
      thr->m_requests.emplace_back(
          req, std::move(work), std::forward_as_tuple(std::forward<U>(u)...));

      // signal the thread
      thr->m_cond.notify_one();

      // return future
      return thr->m_promises.CreateFuture(req);
    }

    // XXX: is this the right thing to do?
    return future<R>();
  }

  /**
   * Wakeup the worker thread, call the work function, and call the afterWork
   * function with the result on the loop set by SetLoop().
   *
   * It’s safe to call this function from any thread.
   * The work function will be called on the worker thread, and the afterWork
   * function will be called on the loop thread.
   *
   * SetLoop() must be called prior to calling this function for afterWork to
   * be called.
   *
   * @param work Work function (called on worker thread)
   * @param afterWork After work function (called on loop thread)
   */
  template <typename... U>
  void QueueWorkThen(WorkFunction work, AfterWorkFunction afterWork, U&&... u) {
    if (auto thr = m_owner.GetThread()) {
      // add the parameters to the input queue
      thr->m_requests.emplace_back(
          std::move(work), std::move(afterWork),
          std::forward_as_tuple(std::forward<U>(u)...));

      // signal the thread
      thr->m_cond.notify_one();
    }
  }

 private:
  SafeThreadOwner<Thread> m_owner;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_WORKERTHREAD_H_
