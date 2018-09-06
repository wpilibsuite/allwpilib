/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_FUTURE_H_
#define WPIUTIL_WPI_FUTURE_H_

#include <stdint.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

#include "wpi/condition_variable.h"
#include "wpi/mutex.h"

namespace wpi {

template <typename T>
class PromiseFactory;

template <typename T>
class future;

template <typename T>
class promise;
template <>
class promise<void>;

namespace detail {

class PromiseFactoryBase {
 public:
  ~PromiseFactoryBase();

  bool IsActive() const { return m_active; }

  wpi::mutex& GetResultMutex() { return m_resultMutex; }

  void Notify() { m_resultCv.notify_all(); }

  // must be called with locked lock == ResultMutex
  void Wait(std::unique_lock<wpi::mutex>& lock) { m_resultCv.wait(lock); }

  // returns false if timeout reached
  template <class Clock, class Duration>
  bool WaitUntil(std::unique_lock<wpi::mutex>& lock,
                 const std::chrono::time_point<Clock, Duration>& timeout_time) {
    return m_resultCv.wait_until(lock, timeout_time) ==
           std::cv_status::no_timeout;
  }

  void IgnoreResult(uint64_t request);

  uint64_t CreateRequest();

  // returns true if request was pending
  // must be called with ResultMutex held
  bool EraseRequest(uint64_t request);

  // same as doing CreateRequest() followed by EraseRequest()
  // must be called with ResultMutex held
  uint64_t CreateErasedRequest() { return ++m_uid; }

 private:
  wpi::mutex m_resultMutex;
  std::atomic_bool m_active{true};
  wpi::condition_variable m_resultCv;

  uint64_t m_uid = 0;
  std::vector<uint64_t> m_requests;
};

template <typename To, typename From>
struct FutureThen {
  template <typename F>
  static future<To> Create(PromiseFactory<From>& fromFactory, uint64_t request,
                           PromiseFactory<To>& factory, F&& func);
};

template <typename From>
struct FutureThen<void, From> {
  template <typename F>
  static future<void> Create(PromiseFactory<From>& fromFactory,
                             uint64_t request, PromiseFactory<void>& factory,
                             F&& func);
};

template <typename To>
struct FutureThen<To, void> {
  template <typename F>
  static future<To> Create(PromiseFactory<void>& fromFactory, uint64_t request,
                           PromiseFactory<To>& factory, F&& func);
};

template <>
struct FutureThen<void, void> {
  template <typename F>
  static future<void> Create(PromiseFactory<void>& fromFactory,
                             uint64_t request, PromiseFactory<void>& factory,
                             F&& func);
};

}  // namespace detail

/**
 * A promise factory for lightweight futures.
 *
 * The lifetime of the factory must be ensured to be longer than any futures
 * it creates.
 *
 * Use CreateRequest() to create the future request id, and then CreateFuture()
 * and CreatePromise() to create future and promise objects.  A promise should
 * only be created once for any given request id.
 *
 * @tparam T the "return" type of the promise/future
 */
template <typename T>
class PromiseFactory final : public detail::PromiseFactoryBase {
  friend class future<T>;

 public:
  using detail::PromiseFactoryBase::Notify;
  using ThenFunction = std::function<void(uint64_t, T)>;

  /**
   * Creates a future.
   *
   * @param request the request id returned by CreateRequest()
   * @return the future
   */
  future<T> CreateFuture(uint64_t request);

  /**
   * Creates a future and makes it immediately ready.
   *
   * @return the future
   */
  future<T> MakeReadyFuture(T&& value);

  /**
   * Creates a promise.
   *
   * @param request the request id returned by CreateRequest()
   * @return the promise
   */
  promise<T> CreatePromise(uint64_t request);

  /**
   * Sets a value directly for a future without creating a promise object.
   * Identical to `CreatePromise(request).set_value(value)`.
   *
   * @param request request id, as returned by CreateRequest()
   * @param value lvalue
   */
  void SetValue(uint64_t request, const T& value);

  /**
   * Sets a value directly for a future without creating a promise object.
   * Identical to `CreatePromise(request).set_value(value)`.
   *
   * @param request request id, as returned by CreateRequest()
   * @param value rvalue
   */
  void SetValue(uint64_t request, T&& value);

  void SetThen(uint64_t request, uint64_t outRequest, ThenFunction func);

  bool IsReady(uint64_t request) noexcept;
  T GetResult(uint64_t request);
  void WaitResult(uint64_t request);
  template <class Clock, class Duration>
  bool WaitResultUntil(
      uint64_t request,
      const std::chrono::time_point<Clock, Duration>& timeout_time);

  static PromiseFactory& GetInstance();

 private:
  struct Then {
    Then(uint64_t request_, uint64_t outRequest_, ThenFunction func_)
        : request(request_), outRequest(outRequest_), func(std::move(func_)) {}
    uint64_t request;
    uint64_t outRequest;
    ThenFunction func;
  };

  std::vector<Then> m_thens;
  std::vector<std::pair<uint64_t, T>> m_results;
};

/**
 * Explicit specialization for PromiseFactory<void>.
 */
template <>
class PromiseFactory<void> final : public detail::PromiseFactoryBase {
  friend class future<void>;

 public:
  using detail::PromiseFactoryBase::Notify;
  using ThenFunction = std::function<void(uint64_t)>;

  /**
   * Creates a future.
   *
   * @param request the request id returned by CreateRequest()
   * @return std::pair of the future and the request id
   */
  future<void> CreateFuture(uint64_t request);

  /**
   * Creates a future and makes it immediately ready.
   *
   * @return the future
   */
  future<void> MakeReadyFuture();

  /**
   * Creates a promise.
   *
   * @param request the request id returned by CreateRequest()
   * @return the promise
   */
  promise<void> CreatePromise(uint64_t request);

  /**
   * Sets a value directly for a future without creating a promise object.
   * Identical to `promise(factory, request).set_value()`.
   *
   * @param request request id, as returned by CreateRequest()
   */
  void SetValue(uint64_t request);

  void SetThen(uint64_t request, uint64_t outRequest, ThenFunction func);

  bool IsReady(uint64_t request) noexcept;
  void GetResult(uint64_t request);
  void WaitResult(uint64_t request);
  template <class Clock, class Duration>
  bool WaitResultUntil(
      uint64_t request,
      const std::chrono::time_point<Clock, Duration>& timeout_time);

  static PromiseFactory& GetInstance();

 private:
  struct Then {
    Then(uint64_t request_, uint64_t outRequest_, ThenFunction func_)
        : request(request_), outRequest(outRequest_), func(std::move(func_)) {}
    uint64_t request;
    uint64_t outRequest;
    ThenFunction func;
  };

  std::vector<Then> m_thens;
  std::vector<uint64_t> m_results;
};

/**
 * A lightweight version of std::future.
 *
 * Use either promise::get_future() or PromiseFactory::CreateFuture() to create.
 *
 * @tparam T the "return" type
 */
template <typename T>
class future final {
  friend class PromiseFactory<T>;
  friend class promise<T>;

 public:
  /**
   * Constructs an empty (invalid) future.
   */
  future() noexcept = default;

  future(future&& oth) noexcept {
    this->m_request = oth.m_request;
    this->m_promises = oth.m_promises;
    oth.m_request = 0;
    oth.m_promises = nullptr;
  }
  future(const future&) = delete;

  template <typename R>
  future(future<R>&& oth) noexcept
      : future(oth.then([](R&& val) -> T { return val; })) {}

  /**
   * Ignores the result of the future if it has not been retrieved.
   */
  ~future() {
    if (m_promises) m_promises->IgnoreResult(m_request);
  }

  future& operator=(future&& oth) noexcept {
    this->m_request = oth.m_request;
    this->m_promises = oth.m_promises;
    oth.m_request = 0;
    oth.m_promises = nullptr;
    return *this;
  }
  future& operator=(const future&) = delete;

  /**
   * Gets the value.  Calls wait() if the value is not yet available.
   * Can only be called once.  The future will be marked invalid after the call.
   *
   * @return The value provided by the corresponding promise.set_value().
   */
  T get() {
    if (m_promises)
      return m_promises->GetResult(m_request);
    else
      return T();
  }

  template <typename R, typename F>
  future<R> then(PromiseFactory<R>& factory, F&& func) {
    if (m_promises) {
      auto promises = m_promises;
      m_promises = nullptr;
      return detail::FutureThen<R, T>::Create(*promises, m_request, factory,
                                              func);
    } else {
      return future<R>();
    }
  }

  template <typename F, typename R = typename std::result_of<F && (T &&)>::type>
  future<R> then(F&& func) {
    return then(PromiseFactory<R>::GetInstance(), std::forward<F>(func));
  }

  bool is_ready() const noexcept {
    return m_promises && m_promises->IsReady(m_request);
  }

  /**
   * Checks if the future is valid.
   * A default-constructed future or one where get() has been called is invalid.
   *
   * @return True if valid
   */
  bool valid() const noexcept { return m_promises; }

  /**
   * Waits for the promise to provide a value.
   * Does not return until the value is available or the promise is destroyed
   * (in which case a default-constructed value is "returned").
   * If the value has already been provided, returns immediately.
   */
  void wait() const {
    if (m_promises) m_promises->WaitResult(m_request);
  }

  /**
   * Waits for the promise to provide a value, or the specified time has been
   * reached.
   *
   * @return True if the promise provided a value, false if timed out.
   */
  template <class Clock, class Duration>
  bool wait_until(
      const std::chrono::time_point<Clock, Duration>& timeout_time) const {
    return m_promises && m_promises->WaitResultUntil(m_request, timeout_time);
  }

  /**
   * Waits for the promise to provide a value, or the specified amount of time
   * has elapsed.
   *
   * @return True if the promise provided a value, false if timed out.
   */
  template <class Rep, class Period>
  bool wait_for(
      const std::chrono::duration<Rep, Period>& timeout_duration) const {
    return wait_until(std::chrono::steady_clock::now() + timeout_duration);
  }

 private:
  future(PromiseFactory<T>* promises, uint64_t request) noexcept
      : m_request(request), m_promises(promises) {}

  uint64_t m_request = 0;
  PromiseFactory<T>* m_promises = nullptr;
};

/**
 * Explicit specialization for future<void>.
 */
template <>
class future<void> final {
  friend class PromiseFactory<void>;
  friend class promise<void>;

 public:
  /**
   * Constructs an empty (invalid) future.
   */
  future() noexcept = default;

  future(future&& oth) noexcept {
    m_request = oth.m_request;
    m_promises = oth.m_promises;
    oth.m_request = 0;
    oth.m_promises = nullptr;
  }
  future(const future&) = delete;

  /**
   * Ignores the result of the future if it has not been retrieved.
   */
  ~future() {
    if (m_promises) m_promises->IgnoreResult(m_request);
  }

  future& operator=(future&& oth) noexcept {
    m_request = oth.m_request;
    m_promises = oth.m_promises;
    oth.m_request = 0;
    oth.m_promises = nullptr;
    return *this;
  }
  future& operator=(const future&) = delete;

  /**
   * Gets the value.  Calls wait() if the value is not yet available.
   * Can only be called once.  The future will be marked invalid after the call.
   */
  void get() {
    if (m_promises) m_promises->GetResult(m_request);
  }

  template <typename R, typename F>
  future<R> then(PromiseFactory<R>& factory, F&& func) {
    if (m_promises) {
      auto promises = m_promises;
      m_promises = nullptr;
      return detail::FutureThen<R, void>::Create(*promises, m_request, factory,
                                                 func);
    } else {
      return future<R>();
    }
  }

  template <typename F, typename R = typename std::result_of<F && ()>::type>
  future<R> then(F&& func) {
    return then(PromiseFactory<R>::GetInstance(), std::forward<F>(func));
  }

  bool is_ready() const noexcept {
    return m_promises && m_promises->IsReady(m_request);
  }

  /**
   * Checks if the future is valid.
   * A default-constructed future or one where get() has been called is invalid.
   *
   * @return True if valid
   */
  bool valid() const noexcept { return m_promises; }

  /**
   * Waits for the promise to provide a value.
   * Does not return until the value is available or the promise is destroyed
   * If the value has already been provided, returns immediately.
   */
  void wait() const {
    if (m_promises) m_promises->WaitResult(m_request);
  }

  /**
   * Waits for the promise to provide a value, or the specified time has been
   * reached.
   *
   * @return True if the promise provided a value, false if timed out.
   */
  template <class Clock, class Duration>
  bool wait_until(
      const std::chrono::time_point<Clock, Duration>& timeout_time) const {
    return m_promises && m_promises->WaitResultUntil(m_request, timeout_time);
  }

  /**
   * Waits for the promise to provide a value, or the specified amount of time
   * has elapsed.
   *
   * @return True if the promise provided a value, false if timed out.
   */
  template <class Rep, class Period>
  bool wait_for(
      const std::chrono::duration<Rep, Period>& timeout_duration) const {
    return wait_until(std::chrono::steady_clock::now() + timeout_duration);
  }

 private:
  future(PromiseFactory<void>* promises, uint64_t request) noexcept
      : m_request(request), m_promises(promises) {}

  uint64_t m_request = 0;
  PromiseFactory<void>* m_promises = nullptr;
};

/**
 * A lightweight version of std::promise.
 *
 * Use PromiseFactory::CreatePromise() to create.
 *
 * @tparam T the "return" type
 */
template <typename T>
class promise final {
  friend class PromiseFactory<T>;

 public:
  /**
   * Constructs an empty promise.
   */
  promise() : m_promises(&PromiseFactory<T>::GetInstance()) {
    m_request = m_promises->CreateRequest();
  }

  promise(promise&& oth) noexcept
      : m_request(oth.m_request), m_promises(oth.m_promises) {
    oth.m_request = 0;
    oth.m_promises = nullptr;
  }

  promise(const promise&) = delete;

  /**
   * Sets the promised value to a default-constructed T if not already set.
   */
  ~promise() {
    if (m_promises) m_promises->SetValue(m_request, T());
  }

  promise& operator=(promise&& oth) noexcept {
    m_request = oth.m_request;
    m_promises = oth.m_promises;
    oth.m_request = 0;
    oth.m_promises = nullptr;
    return *this;
  }

  promise& operator=(const promise&) = delete;

  /**
   * Swaps this promise with another one.
   */
  void swap(promise& oth) noexcept {
    std::swap(m_request, oth.m_request);
    std::swap(m_promises, oth.m_promises);
  }

  /**
   * Gets a future for this promise.
   *
   * @return The future
   */
  future<T> get_future() noexcept { return future<T>(m_promises, m_request); }

  /**
   * Sets the promised value.
   * Only effective once (subsequent calls will be ignored).
   *
   * @param value The value to provide to the waiting future
   */
  void set_value(const T& value) {
    if (m_promises) m_promises->SetValue(m_request, value);
    m_promises = nullptr;
  }

  /**
   * Sets the promised value.
   * Only effective once (subsequent calls will be ignored).
   *
   * @param value The value to provide to the waiting future
   */
  void set_value(T&& value) {
    if (m_promises) m_promises->SetValue(m_request, std::move(value));
    m_promises = nullptr;
  }

 private:
  promise(PromiseFactory<T>* promises, uint64_t request) noexcept
      : m_request(request), m_promises(promises) {}

  uint64_t m_request = 0;
  PromiseFactory<T>* m_promises = nullptr;
};

/**
 * Explicit specialization for promise<void>.
 */
template <>
class promise<void> final {
  friend class PromiseFactory<void>;

 public:
  /**
   * Constructs an empty promise.
   */
  promise() : m_promises(&PromiseFactory<void>::GetInstance()) {
    m_request = m_promises->CreateRequest();
  }

  promise(promise&& oth) noexcept
      : m_request(oth.m_request), m_promises(oth.m_promises) {
    oth.m_request = 0;
    oth.m_promises = nullptr;
  }

  promise(const promise&) = delete;

  /**
   * Sets the promised value if not already set.
   */
  ~promise() {
    if (m_promises) m_promises->SetValue(m_request);
  }

  promise& operator=(promise&& oth) noexcept {
    m_request = oth.m_request;
    m_promises = oth.m_promises;
    oth.m_request = 0;
    oth.m_promises = nullptr;
    return *this;
  }

  promise& operator=(const promise&) = delete;

  /**
   * Swaps this promise with another one.
   */
  void swap(promise& oth) noexcept {
    std::swap(m_request, oth.m_request);
    std::swap(m_promises, oth.m_promises);
  }

  /**
   * Gets a future for this promise.
   *
   * @return The future
   */
  future<void> get_future() noexcept {
    return future<void>(m_promises, m_request);
  }

  /**
   * Sets the promised value.
   * Only effective once (subsequent calls will be ignored).
   */
  void set_value() {
    if (m_promises) m_promises->SetValue(m_request);
    m_promises = nullptr;
  }

 private:
  promise(PromiseFactory<void>* promises, uint64_t request) noexcept
      : m_request(request), m_promises(promises) {}

  uint64_t m_request = 0;
  PromiseFactory<void>* m_promises = nullptr;
};

/**
 * Constructs a valid future with the value set.
 */
template <typename T>
inline future<T> make_ready_future(T&& value) {
  return PromiseFactory<T>::GetInstance().MakeReadyFuture(
      std::forward<T>(value));
}

/**
 * Constructs a valid future with the value set.
 */
inline future<void> make_ready_future() {
  return PromiseFactory<void>::GetInstance().MakeReadyFuture();
}

template <typename T>
inline future<T> PromiseFactory<T>::CreateFuture(uint64_t request) {
  return future<T>{this, request};
}

template <typename T>
future<T> PromiseFactory<T>::MakeReadyFuture(T&& value) {
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  uint64_t req = CreateErasedRequest();
  m_results.emplace_back(std::piecewise_construct, std::forward_as_tuple(req),
                         std::forward_as_tuple(std::move(value)));
  return future<T>{this, req};
}

template <typename T>
inline promise<T> PromiseFactory<T>::CreatePromise(uint64_t request) {
  return promise<T>{this, request};
}

template <typename T>
void PromiseFactory<T>::SetValue(uint64_t request, const T& value) {
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  if (!EraseRequest(request)) return;
  auto it = std::find_if(m_thens.begin(), m_thens.end(),
                         [=](const auto& x) { return x.request == request; });
  if (it != m_thens.end()) {
    uint64_t outRequest = it->outRequest;
    ThenFunction func = std::move(it->func);
    m_thens.erase(it);
    lock.unlock();
    return func(outRequest, value);
  }
  m_results.emplace_back(std::piecewise_construct,
                         std::forward_as_tuple(request),
                         std::forward_as_tuple(value));
  Notify();
}

template <typename T>
void PromiseFactory<T>::SetValue(uint64_t request, T&& value) {
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  if (!EraseRequest(request)) return;
  auto it = std::find_if(m_thens.begin(), m_thens.end(),
                         [=](const auto& x) { return x.request == request; });
  if (it != m_thens.end()) {
    uint64_t outRequest = it->outRequest;
    ThenFunction func = std::move(it->func);
    m_thens.erase(it);
    lock.unlock();
    return func(outRequest, std::move(value));
  }
  m_results.emplace_back(std::piecewise_construct,
                         std::forward_as_tuple(request),
                         std::forward_as_tuple(std::move(value)));
  Notify();
}

template <typename T>
void PromiseFactory<T>::SetThen(uint64_t request, uint64_t outRequest,
                                ThenFunction func) {
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  auto it = std::find_if(m_results.begin(), m_results.end(),
                         [=](const auto& r) { return r.first == request; });
  if (it != m_results.end()) {
    auto val = std::move(it->second);
    m_results.erase(it);
    lock.unlock();
    return func(outRequest, std::move(val));
  }
  m_thens.emplace_back(request, outRequest, func);
}

template <typename T>
bool PromiseFactory<T>::IsReady(uint64_t request) noexcept {
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  auto it = std::find_if(m_results.begin(), m_results.end(),
                         [=](const auto& r) { return r.first == request; });
  return it != m_results.end();
}

template <typename T>
T PromiseFactory<T>::GetResult(uint64_t request) {
  // wait for response
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  while (IsActive()) {
    // Did we get a response to *our* request?
    auto it = std::find_if(m_results.begin(), m_results.end(),
                           [=](const auto& r) { return r.first == request; });
    if (it != m_results.end()) {
      // Yes, remove it from the vector and we're done.
      auto rv = std::move(it->second);
      m_results.erase(it);
      return rv;
    }
    // No, keep waiting for a response
    Wait(lock);
  }
  return T();
}

template <typename T>
void PromiseFactory<T>::WaitResult(uint64_t request) {
  // wait for response
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  while (IsActive()) {
    // Did we get a response to *our* request?
    auto it = std::find_if(m_results.begin(), m_results.end(),
                           [=](const auto& r) { return r.first == request; });
    if (it != m_results.end()) return;
    // No, keep waiting for a response
    Wait(lock);
  }
}

template <typename T>
template <class Clock, class Duration>
bool PromiseFactory<T>::WaitResultUntil(
    uint64_t request,
    const std::chrono::time_point<Clock, Duration>& timeout_time) {
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  bool timeout = false;
  while (IsActive()) {
    // Did we get a response to *our* request?
    auto it = std::find_if(m_results.begin(), m_results.end(),
                           [=](const auto& r) { return r.first == request; });
    if (it != m_results.end()) return true;
    if (timeout) break;
    // No, keep waiting for a response
    if (!WaitUntil(lock, timeout_time)) timeout = true;
  }
  return false;
}

template <typename T>
PromiseFactory<T>& PromiseFactory<T>::GetInstance() {
  static PromiseFactory inst;
  return inst;
}

inline future<void> PromiseFactory<void>::CreateFuture(uint64_t request) {
  return future<void>{this, request};
}

inline promise<void> PromiseFactory<void>::CreatePromise(uint64_t request) {
  return promise<void>{this, request};
}

template <class Clock, class Duration>
bool PromiseFactory<void>::WaitResultUntil(
    uint64_t request,
    const std::chrono::time_point<Clock, Duration>& timeout_time) {
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  bool timeout = false;
  while (IsActive()) {
    // Did we get a response to *our* request?
    auto it = std::find_if(m_results.begin(), m_results.end(),
                           [=](const auto& r) { return r == request; });
    if (it != m_results.end()) return true;
    if (timeout) break;
    // No, keep waiting for a response
    if (!WaitUntil(lock, timeout_time)) timeout = true;
  }
  return false;
}

template <typename To, typename From>
template <typename F>
future<To> detail::FutureThen<To, From>::Create(
    PromiseFactory<From>& fromFactory, uint64_t request,
    PromiseFactory<To>& factory, F&& func) {
  uint64_t req = factory.CreateRequest();
  fromFactory.SetThen(request, req, [&factory, func](uint64_t r, From value) {
    factory.SetValue(r, func(std::move(value)));
  });
  return factory.CreateFuture(req);
}

template <typename From>
template <typename F>
future<void> detail::FutureThen<void, From>::Create(
    PromiseFactory<From>& fromFactory, uint64_t request,
    PromiseFactory<void>& factory, F&& func) {
  uint64_t req = factory.CreateRequest();
  fromFactory.SetThen(request, req, [&factory, func](uint64_t r, From value) {
    func(std::move(value));
    factory.SetValue(r);
  });
  return factory.CreateFuture(req);
}

template <typename To>
template <typename F>
future<To> detail::FutureThen<To, void>::Create(
    PromiseFactory<void>& fromFactory, uint64_t request,
    PromiseFactory<To>& factory, F&& func) {
  uint64_t req = factory.CreateRequest();
  fromFactory.SetThen(request, req, [&factory, func](uint64_t r) {
    factory.SetValue(r, func());
  });
  return factory.CreateFuture(req);
}

template <typename F>
future<void> detail::FutureThen<void, void>::Create(
    PromiseFactory<void>& fromFactory, uint64_t request,
    PromiseFactory<void>& factory, F&& func) {
  uint64_t req = factory.CreateRequest();
  fromFactory.SetThen(request, req, [&factory, func](uint64_t r) {
    func();
    factory.SetValue(r);
  });
  return factory.CreateFuture(req);
}

}  // namespace wpi

#endif  // WPIUTIL_WPI_FUTURE_H_
