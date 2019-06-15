/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
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
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "wpi/condition_variable.h"
#include "wpi/mutex.h"

namespace wpi {

template <typename T>
class PromiseFactory;

template <typename T, class E>
class continuable_future;
template <typename T>
class future;

template <typename T>
class promise;

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

/**
 * If U is std::future<T>, provides the member constant value equal to true.
 * Otherwise value is false.
 */
template <typename U>
struct is_future : std::false_type {};
template <typename T>
struct is_future<future<T>> : std::true_type {};

/**
 * Helper variable template for is_future<U>.
 */
template <typename U>
inline constexpr bool is_future_v = is_future<U>::value;

/**
 * If U is a continuable_future<T, E>, provides T as the member type.
 * Otherwise provides U as the member type.
 */
template <typename U>
struct cont_future_unwrap {
  using type = U;
};
template <typename T, typename E>
struct cont_future_unwrap<continuable_future<T, E>> {
  using type = T;
};

/**
 * Helper type template for cont_future_unwrap<U>.
 */
template <typename U>
using cont_future_unwrap_t = typename cont_future_unwrap<U>::type;

/**
 * If U is a future<T>, provides T as the member type.
 * Otherwise the member type is not defined.
 */
template <typename U>
struct future_unwrap {};
template <typename T>
struct future_unwrap<future<T>> {
  using type = T;
};

/**
 * Helper type template for future_unwrap<U>.
 */
template <typename U>
using future_unwrap_t = typename future_unwrap<U>::type;

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
  using ThenFunction =
      std::function<void(void*, uint64_t, std::shared_ptr<void>, T)>;

  /**
   * Creates a future.
   *
   * @param request the request id returned by CreateRequest()
   * @return the future
   */
  future<T> CreateFuture(uint64_t request);

  /**
   * Creates a continuable_future.
   *
   * @param request the request id returned by CreateRequest()
   * @param exe the executor
   * @return the future
   */
  template <typename E>
  continuable_future<T, E> CreateContinuableFuture(uint64_t request,
                                                   const E& exe);

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

  void SetThen(uint64_t request, void* outFactory, uint64_t outRequest,
               std::shared_ptr<void> executor, ThenFunction func);

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
    template <typename F>
    Then(uint64_t request_, void* outFactory_, uint64_t outRequest_,
         std::shared_ptr<void> executor_, F&& func_)
        : request(request_),
          outRequest(outRequest_),
          outFactory(outFactory_),
          executor(std::move(executor_)),
          func(std::forward<F>(func_)) {}
    uint64_t request;
    uint64_t outRequest;
    void* outFactory;
    std::shared_ptr<void> executor;
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
  using ThenFunction =
      std::function<void(void*, uint64_t, std::shared_ptr<void>)>;

  /**
   * Creates a future.
   *
   * @param request the request id returned by CreateRequest()
   * @return std::pair of the future and the request id
   */
  future<void> CreateFuture(uint64_t request);

  /**
   * Creates a continuable_future.
   *
   * @param request the request id returned by CreateRequest()
   * @param exe the executor
   * @return the future
   */
  template <typename E>
  continuable_future<void, E> CreateContinuableFuture(uint64_t request,
                                                      const E& exe);

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

  void SetThen(uint64_t request, void* outFactory, uint64_t outRequest,
               std::shared_ptr<void> executor, ThenFunction func);

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
    template <typename F>
    Then(uint64_t request_, void* outFactory_, uint64_t outRequest_,
         std::shared_ptr<void> executor_, F&& func_)
        : request(request_),
          outRequest(outRequest_),
          outFactory(outFactory_),
          executor(std::move(executor_)),
          func(std::forward<F>(func_)) {}
    uint64_t request;
    uint64_t outRequest;
    void* outFactory;
    std::shared_ptr<void> executor;
    ThenFunction func;
  };

  std::vector<Then> m_thens;
  std::vector<uint64_t> m_results;
};

/**
 * A lightweight variant of std::execution::semi_future.
 * This can be used directly or bound to an executor using the via() function
 * to produce a continuable_future.
 *
 * Use make_promise_contract() or PromiseFactory::CreateFuture() to create.
 *
 * @tparam T the "return" type
 */
template <typename T>
class future final {
  friend class PromiseFactory<T>;

 public:
  using value_type = T;

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

  /**
   * Conversion constructor from continuable_future<T, E> when T is the same
   * type as this class' T.
   */
  template <typename E>
  future(continuable_future<T, E>&& oth) noexcept {  // NOLINT: runtime/explicit
    std::tie(m_promises, m_request) = std::move(oth).ToFactory();
  }

  /**
   * Conversion constructor from continuable_future<T, E> when T is a different
   * type than this class' T.
   */
  template <typename R, typename E,
            typename = std::enable_if_t<!std::is_same<T, R>::value>>
  future(continuable_future<R, E>&& oth) {  // NOLINT: runtime/explicit
    std::tie(m_promises, m_request) =
        std::move(oth).then([](R&& val) -> T { return val; }).ToFactory();
  }

  /**
   * Unwrapping constructor from continuable_future<future<T>, E>.
   */
  template <typename E>
  future(continuable_future<future<T>, E>&& oth);  // NOLINT: runtime/explicit

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
   * Checks if the future is ready.
   *
   * @return True if get() will return a value immediately, false if it will
   * block.
   */
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
   * Bind to an executor to make a continuable_future.
   */
  template <class E>
  continuable_future<T, E> via(E exe) && {
    if (!m_promises) return continuable_future<T, E>{};
    auto rv = m_promises->CreateContinuableFuture(m_request, exe);
    m_promises = nullptr;
    m_request = 0;
    return rv;
  }

  /**
   * Get the PromiseFactory and request ID for this future.
   * This is a move operation (the future is left in an invalid state).
   */
  std::pair<PromiseFactory<T>*, uint64_t> ToFactory() && noexcept {
    auto rv = GetFactory();
    m_promises = nullptr;
    m_request = 0;
    return rv;
  }

  std::pair<PromiseFactory<T>*, uint64_t> GetFactory() const noexcept {
    return std::make_pair(m_promises, m_request);
  }

 private:
  future(PromiseFactory<T>* promises, uint64_t request) noexcept
      : m_request(request), m_promises(promises) {}

  uint64_t m_request = 0;
  PromiseFactory<T>* m_promises = nullptr;
};

/**
 * A lightweight variant of std::execution::continuable_future.
 * This is a future that is bound to an executor and can have continuations
 * attached to it.
 *
 * Use make_promise_contract(Executor) or
 * PromiseFactory::CreateContinuableFuture() to create.
 *
 * @tparam T the "return" type
 */
template <typename T, typename E>
class continuable_future final {
  friend class PromiseFactory<T>;
  friend class future<T>;
  template <typename T2, typename E2>
  friend class continuable_future;

 public:
  using value_type = T;
  using executor_type = E;
  using future_type = future<T>;

  /**
   * Constructs an empty (invalid) future.
   */
  continuable_future() noexcept = default;

  continuable_future(continuable_future&& oth) noexcept {
    this->m_request = oth.m_request;
    this->m_promises = oth.m_promises;
    this->m_executor = oth.m_executor;
    oth.m_request = 0;
    oth.m_promises = nullptr;
  }
  continuable_future(const continuable_future&) = delete;

  /**
   * Conversion constructor from continuable_future<T, E>; both T and E can
   * be different types than this class' T and E.
   */
  template <typename R, typename EI,
            typename = std::enable_if_t<!std::is_same<T, R>::value>>
  continuable_future(continuable_future<R, EI>&& oth)
      : continuable_future(
            std::move(oth).then([](R&& val) -> T { return val; })) {}

  /**
   * Unwrapping constructor from continuable_future<continuable_future<T, E2>,
   * E>.
   */
  template <typename E2>
  continuable_future(continuable_future<continuable_future<T, E2>, E>&&
                         oth);  // NOLINT: runtime/explicit

  continuable_future(PromiseFactory<T>* promises, uint64_t request,
                     E executor) noexcept
      : m_request(request),
        m_promises(promises),
        m_executor(std::move(executor)) {}

  /**
   * Ignores the result of the future if it has not been retrieved.
   */
  ~continuable_future() {
    if (m_promises) m_promises->IgnoreResult(m_request);
  }

  continuable_future& operator=(continuable_future&& oth) noexcept {
    this->m_request = oth.m_request;
    this->m_promises = oth.m_promises;
    this->m_executor = oth.m_executor;
    oth.m_request = 0;
    oth.m_promises = nullptr;
    return *this;
  }
  continuable_future& operator=(const continuable_future&) = delete;

  /**
   * Checks if the future is ready.
   *
   * @return True if get() will return a value immediately, false if it will
   * block.
   */
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
   * Link a continuation functor to this future.  The functor will get executed
   * with the result of this future when it is made ready.  A continuable_future
   * is returned that represents the future value of the return value of the
   * functor.  The returned continuable_future is bound to the same executor as
   * this continuable_future.
   *
   * Automatic unwrapping is performed if the functor returns a wpi::future or
   * wpi::continuable_future.  This variant of the function handles normal and
   * wpi::continuable_future functor return values.
   */
  template <typename F,
            std::enable_if_t<
                !detail::is_future_v<std::result_of_t<std::decay_t<F>(T&&)>>,
                int> = 0>
  continuable_future<
      detail::cont_future_unwrap_t<std::result_of_t<std::decay_t<F>(T&&)>>, E>
  then(F&& func) && {
    return m_executor
        .template then_execute<std::result_of_t<std::decay_t<F>(T &&)>>(
            func, std::move(*this));
  }

  /**
   * This variant of then() handles wpi::future functor return values.
   */
  template <typename F>
  continuable_future<
      detail::future_unwrap_t<std::result_of_t<std::decay_t<F>(T&&)>>, E>
  then(F&& func) && {
    return future<detail::future_unwrap_t<
        std::result_of_t<std::decay_t<F>(T &&)>>>(
               m_executor.template then_execute<
                   std::result_of_t<std::decay_t<F>(T &&)>>(func,
                                                            std::move(*this)))
        .via(m_executor);
  }

  /**
   * Bind to a different executor.
   */
  template <typename EI>
  continuable_future<T, EI> via(EI exe) && {
    auto h = std::move(*this).ToFactory();
    return continuable_future<T, EI>(h.first, h.second, exe);
  }

  /**
   * Get the executor bound to this future.
   */
  E get_executor() const noexcept { return m_executor; }

  /**
   * Get the unbound future for this future.
   */
  future<T> semi() && noexcept { return future<T>(std::move(*this)); }

  /**
   * Get the PromiseFactory and request ID for this future.
   * This is a move operation (the future is left in an invalid state).
   */
  std::pair<PromiseFactory<T>*, uint64_t> ToFactory() && noexcept {
    auto rv = GetFactory();
    m_promises = nullptr;
    m_request = 0;
    return rv;
  }

  std::pair<PromiseFactory<T>*, uint64_t> GetFactory() const noexcept {
    return std::make_pair(m_promises, m_request);
  }

 private:
  uint64_t m_request = 0;
  PromiseFactory<T>* m_promises = nullptr;
  E m_executor;
};

/**
 * Explicit specialization for continuable_future<void, E>.
 */
template <typename E>
class continuable_future<void, E> final {
  friend class PromiseFactory<void>;
  friend class future<void>;
  template <typename T2, typename E2>
  friend class continuable_future;

 public:
  using value_type = void;
  using executor_type = E;
  using future_type = future<void>;

  /**
   * Constructs an empty (invalid) future.
   */
  continuable_future() noexcept = default;

  continuable_future(continuable_future&& oth) noexcept {
    this->m_request = oth.m_request;
    this->m_promises = oth.m_promises;
    this->m_executor = oth.m_executor;
    oth.m_request = 0;
    oth.m_promises = nullptr;
  }
  continuable_future(const continuable_future&) = delete;

  /**
   * Conversion constructor from continuable_future<T, E>; both T and E can
   * be different types than this class' T and E.
   */
  template <typename R, typename EI,
            typename = std::enable_if_t<!std::is_void_v<R>>>
  continuable_future(continuable_future<R, EI>&& oth)
      : continuable_future(std::move(oth).then([](R&& val) {})) {}

  /**
   * Unwrapping constructor from continuable_future<continuable_future<T, E2>,
   * E>.
   */
  template <typename E2>
  continuable_future(continuable_future<continuable_future<void, E2>, E>&&
                         oth);  // NOLINT: runtime/explicit

  /**
   * Ignores the result of the future if it has not been retrieved.
   */
  ~continuable_future() {
    if (m_promises) m_promises->IgnoreResult(m_request);
  }

  continuable_future& operator=(continuable_future&& oth) noexcept {
    this->m_request = oth.m_request;
    this->m_promises = oth.m_promises;
    this->m_executor = oth.m_executor;
    oth.m_request = 0;
    oth.m_promises = nullptr;
    return *this;
  }
  continuable_future& operator=(const continuable_future&) = delete;

  /**
   * Checks if the future is ready.
   *
   * @return True if get() will return a value immediately, false if it will
   * block.
   */
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
   * Link a continuation functor to this future.  The functor will get executed
   * with the result of this future when it is made ready.  A continuable_future
   * is returned that represents the future value of the return value of the
   * functor.  The returned continuable_future is bound to the same executor as
   * this continuable_future.
   *
   * Automatic unwrapping is performed if the functor returns a wpi::future or
   * wpi::continuable_future.  This variant of the function handles normal and
   * wpi::continuable_future functor return values.
   */
  template <
      typename F,
      std::enable_if_t<
          !detail::is_future_v<std::result_of_t<std::decay_t<F>()>>, int> = 0>
  continuable_future<
      detail::cont_future_unwrap_t<std::result_of_t<std::decay_t<F>()>>, E>
  then(F&& func) && {
    return m_executor
        .template then_execute<std::result_of_t<std::decay_t<F>()>>(
            func, std::move(*this));
  }

  /**
   * This variant of then() handles wpi::future functor return values.
   */
  template <typename F>
  continuable_future<
      detail::future_unwrap_t<std::result_of_t<std::decay_t<F>()>>, E>
  then(F&& func) && {
    return future<detail::future_unwrap_t<std::result_of_t<std::decay_t<F>()>>>(
               m_executor
                   .template then_execute<std::result_of_t<std::decay_t<F>()>>(
                       func, std::move(*this)))
        .via(m_executor);
  }

  /**
   * Bind to a different executor.
   */
  template <typename EI>
  continuable_future<void, EI> via(EI exe) && {
    auto h = std::move(*this).ToFactory();
    return continuable_future<void, EI>(h.first, h.second, exe);
  }

  /**
   * Get the executor bound to this future.
   */
  E get_executor() const noexcept { return m_executor; }

  /**
   * Get the unbound future for this future.
   */
  future<void> semi() && noexcept { return future<void>(std::move(*this)); }

  /**
   * Get the PromiseFactory and request ID for this future.
   * This is a move operation (the future is left in an invalid state).
   */
  std::pair<PromiseFactory<void>*, uint64_t> ToFactory() && noexcept {
    auto rv = GetFactory();
    m_promises = nullptr;
    m_request = 0;
    return rv;
  }

  std::pair<PromiseFactory<void>*, uint64_t> GetFactory() const noexcept {
    return std::make_pair(m_promises, m_request);
  }

 private:
  continuable_future(PromiseFactory<void>* promises, uint64_t request,
                     E executor) noexcept
      : m_request(request),
        m_promises(promises),
        m_executor(std::move(executor)) {}

  uint64_t m_request = 0;
  PromiseFactory<void>* m_promises = nullptr;
  E m_executor;
};

/**
 * A lightweight variant of std::execution::promise.
 *
 * Use make_promise_contract() or PromiseFactory::CreatePromise() to create.
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
  promise() noexcept = default;

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
   * Sets the promised value.
   * Only effective once (subsequent calls will be ignored).
   *
   * @param value The value to provide to the waiting future
   */
  void set_value(const T& value) && {
    if (m_promises) m_promises->SetValue(m_request, value);
    m_promises = nullptr;
  }

  /**
   * Sets the promised value.
   * Only effective once (subsequent calls will be ignored).
   *
   * @param value The value to provide to the waiting future
   */
  void set_value(T&& value) && {
    if (m_promises) m_promises->SetValue(m_request, std::move(value));
    m_promises = nullptr;
  }

  bool valid() const noexcept { return m_promises != nullptr; }

  explicit operator bool() const noexcept { return valid(); }

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
  promise() noexcept = default;

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
   * Sets the promised value.
   * Only effective once (subsequent calls will be ignored).
   */
  void set_value() && {
    if (m_promises) m_promises->SetValue(m_request);
    m_promises = nullptr;
  }

  bool valid() const noexcept { return m_promises != nullptr; }

  explicit operator bool() const noexcept { return valid(); }

 private:
  promise(PromiseFactory<void>* promises, uint64_t request) noexcept
      : m_request(request), m_promises(promises) {}

  uint64_t m_request = 0;
  PromiseFactory<void>* m_promises = nullptr;
};

/**
 * An executor that supports inline (e.g. in the same thread) execution of
 * futures/promises.
 */
class inline_executor final {
 public:
  /**
   * One-way execution: calls the function.
   */
  template <typename F>
  void execute(F&& func) const {
    func();
  }

  /**
   * Two-way execution: returns a future for the result of the function.  For
   * this executor, the function is called immediately and the returned future
   * is immediately ready.
   */
  template <typename F,
            typename R = typename std::result_of_t<std::decay_t<F>()>>
  future<R> twoway_execute(F&& func) const;

  /**
   * Continuation (then) execution: returns a future for the result of the
   * function; the function is called after the passed predicate future is made
   * ready.  For this executor, the function is called in the same thread as the
   * predicate future.
   */
  template <typename R, typename F, typename Future>
  continuable_future<R, inline_executor> then_execute(F&& func,
                                                      Future&& pred) const;
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

/**
 * Constructs a promise/future pair.
 */
template <typename T>
inline std::pair<promise<T>, future<T>> make_promise_contract() {
  auto& factory = PromiseFactory<T>::GetInstance();
  uint64_t request = factory.CreateRequest();
  return std::pair(factory.CreatePromise(request),
                   factory.CreateFuture(request));
}

/**
 * Constructs a promise/continuable_future pair from an executor.
 */
template <typename T, typename Executor>
inline std::pair<promise<T>, continuable_future<T, std::decay_t<Executor>>>
make_promise_contract(const Executor& exe) {
  auto& factory = PromiseFactory<T>::GetInstance();
  uint64_t request = factory.CreateRequest();
  return std::pair(factory.CreatePromise(request),
                   factory.CreateContinuableFuture(request, exe));
}

/**
 * Gets the value of a future.  Calls future_wait() if the value is not yet
 * available. Can only be called once.  The future will be marked invalid after
 * the call.
 *
 * @return The value provided by the corresponding promise.set_value().
 */
template <typename Future>
inline typename Future::value_type future_get(Future&& f) {
  auto h = std::move(f).ToFactory();
  if (h.first)
    return h.first->GetResult(h.second);
  else
    return typename Future::value_type();
}

template <typename Future, std::enable_if_t<Future::value_type, void> = 0>
inline void future_get(Future&& f) {
  auto h = std::move(f).ToFactory();
  if (h.first) h.first->GetResult(h.second);
}

/**
 * Waits for the promise to provide a value.
 * Does not return until the value is available or the promise is destroyed
 * (in which case a default-constructed value is "returned").
 * If the value has already been provided, returns immediately.
 */
template <typename Future>
inline void future_wait(Future& f) {
  auto h = f.GetFactory();
  if (h.first) h.first->WaitResult(h.second);
}

/**
 * Waits for the promise to provide a value, or the specified time has been
 * reached.
 *
 * @return True if the promise provided a value, false if timed out.
 */
template <typename Future, class Clock, class Duration>
inline bool future_wait_until(
    Future& f, const std::chrono::time_point<Clock, Duration>& timeout_time) {
  auto h = f.GetFactory();
  return h.first && h.first->WaitResultUntil(h.second, timeout_time);
}

/**
 * Waits for the promise to provide a value, or the specified amount of time
 * has elapsed.
 *
 * @return True if the promise provided a value, false if timed out.
 */
template <typename Future, class Rep, class Period>
inline bool future_wait_for(
    Future& f, const std::chrono::duration<Rep, Period>& timeout_duration) {
  return future_wait_until(f,
                           std::chrono::steady_clock::now() + timeout_duration);
}

/**
 * Execute a function asynchronously on an executor, returning a std::future
 * for the return value of the function.
 *
 * Automatic unwrapping is performed if the functor returns a wpi::future or
 * wpi::continuable_future.  This variant of the function handles normal and
 * wpi::continuable_future functor return values.
 */
template <
    typename Executor, typename F,
    std::enable_if_t<!detail::is_future_v<std::result_of_t<std::decay_t<F>()>>,
                     int> = 0>
inline future<std::result_of_t<std::decay_t<F>()>> async(const Executor& exe,
                                                         F&& func) {
  return exe.twoway_execute(std::forward<F>(func));
}

/**
 * This variant of async() handles wpi::future functor return values.
 */
template <typename Executor, typename F>
inline future<detail::future_unwrap_t<std::result_of_t<std::decay_t<F>()>>>
async(const Executor& exe, F&& func) {
  return exe.twoway_execute(std::forward<F>(func)).via(exe);
}

}  // namespace wpi

#include "future.inl"

#endif  // WPIUTIL_WPI_FUTURE_H_
