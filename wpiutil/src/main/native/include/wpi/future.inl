/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_FUTURE_INL_
#define WPIUTIL_WPI_FUTURE_INL_

namespace wpi {

template <typename T>
inline future<T> PromiseFactory<T>::CreateFuture(uint64_t request) {
  return future<T>{this, request};
}

template <typename T>
template <typename E>
inline continuable_future<T, E> PromiseFactory<T>::CreateContinuableFuture(
    uint64_t request, const E& exe) {
  return continuable_future<T, E>{this, request, exe};
}

template <typename T>
future<T> PromiseFactory<T>::MakeReadyFuture(T&& value) {
  std::unique_lock lock(GetResultMutex());
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
  std::unique_lock lock(GetResultMutex());
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
  std::unique_lock lock(GetResultMutex());
  if (!EraseRequest(request)) return;
  auto it = std::find_if(m_thens.begin(), m_thens.end(),
                         [=](const auto& x) { return x.request == request; });
  if (it != m_thens.end()) {
    void* outFactory = it->outFactory;
    uint64_t outRequest = it->outRequest;
    std::shared_ptr<void> executor = std::move(it->executor);
    ThenFunction func = std::move(it->func);
    m_thens.erase(it);
    lock.unlock();
    return func(outFactory, outRequest, std::move(executor), std::move(value));
  }
  m_results.emplace_back(std::piecewise_construct,
                         std::forward_as_tuple(request),
                         std::forward_as_tuple(std::move(value)));
  Notify();
}

template <typename T>
void PromiseFactory<T>::SetThen(uint64_t request, void* outFactory,
                                uint64_t outRequest,
                                std::shared_ptr<void> executor,
                                ThenFunction func) {
  std::unique_lock lock(GetResultMutex());
  auto it = std::find_if(m_results.begin(), m_results.end(),
                         [=](const auto& r) { return r.first == request; });
  if (it != m_results.end()) {
    auto val = std::move(it->second);
    m_results.erase(it);
    lock.unlock();
    return func(outFactory, outRequest, std::move(executor), std::move(val));
  }
  m_thens.emplace_back(request, outFactory, outRequest, std::move(executor),
                       std::move(func));
}

template <typename T>
bool PromiseFactory<T>::IsReady(uint64_t request) noexcept {
  std::unique_lock lock(GetResultMutex());
  auto it = std::find_if(m_results.begin(), m_results.end(),
                         [=](const auto& r) { return r.first == request; });
  return it != m_results.end();
}

template <typename T>
T PromiseFactory<T>::GetResult(uint64_t request) {
  // wait for response
  std::unique_lock lock(GetResultMutex());
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
  std::unique_lock lock(GetResultMutex());
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
  std::unique_lock lock(GetResultMutex());
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

template <typename E>
inline continuable_future<void, E>
PromiseFactory<void>::CreateContinuableFuture(uint64_t request, const E& exe) {
  return continuable_future<void, E>{this, request, exe};
}

inline promise<void> PromiseFactory<void>::CreatePromise(uint64_t request) {
  return promise<void>{this, request};
}

template <class Clock, class Duration>
bool PromiseFactory<void>::WaitResultUntil(
    uint64_t request,
    const std::chrono::time_point<Clock, Duration>& timeout_time) {
  std::unique_lock lock(GetResultMutex());
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

template <typename T>
template <typename E>
future<T>::future(continuable_future<future<T>, E>&& oth) {
  using Factory = PromiseFactory<T>;
  this->m_promises = &Factory::GetInstance();
  this->m_request = this->m_promises->CreateRequest();

  std::move(oth).then([req = this->m_request](auto&& f) {
    auto [predFactory, predRequest] = std::move(f).ToFactory();
    auto& factory = Factory::GetInstance();
    if constexpr (std::is_same_v<T, void>) {
      predFactory->SetThen(predRequest, &factory, req, nullptr,
                           [](void* fac, uint64_t r, std::shared_ptr<void>) {
                             static_cast<Factory*>(fac)->SetValue(r);
                           });
    } else {
      predFactory->SetThen(
          predRequest, &factory, req, nullptr,
          [](void* fac, uint64_t r, std::shared_ptr<void>, T&& value) {
            static_cast<Factory*>(fac)->SetValue(r, std::move(value));
          });
    }
  });
}

template <typename T, typename E>
template <typename E2>
continuable_future<T, E>::continuable_future(
    continuable_future<continuable_future<T, E2>, E>&& oth) {
  using Factory = PromiseFactory<T>;
  this->m_promises = &Factory::GetInstance();
  this->m_request = this->m_promises->CreateRequest();
  this->m_executor = oth.m_executor;

  std::move(oth).then([req = this->m_request](auto&& f) {
    auto [predFactory, predRequest] = std::move(f).ToFactory();
    auto& factory = Factory::GetInstance();
    predFactory->SetThen(
        predRequest, &factory, req, nullptr,
        [](void* fac, uint64_t r, std::shared_ptr<void>, T&& value) {
          static_cast<Factory*>(fac)->SetValue(r, std::move(value));
        });
  });
}

template <typename E>
template <typename E2>
continuable_future<void, E>::continuable_future(
    continuable_future<continuable_future<void, E2>, E>&& oth) {
  using Factory = PromiseFactory<void>;
  this->m_promises = &Factory::GetInstance();
  this->m_request = this->m_promises->CreateRequest();
  this->m_executor = oth.m_executor;

  std::move(oth).then([req = this->m_request](auto&& f) {
    auto [predFactory, predRequest] = std::move(f).ToFactory();
    auto& factory = Factory::GetInstance();
    predFactory->SetThen(predRequest, &factory, req, nullptr,
                         [](void* fac, uint64_t r, std::shared_ptr<void>) {
                           static_cast<Factory*>(fac)->SetValue(r);
                         });
  });
}

template <typename F, typename R>
inline future<R> inline_executor::twoway_execute(F&& func) const {
  if constexpr (std::is_void_v<R>) {
    func();
    return make_ready_future();
  } else {
    return make_ready_future(func());
  }
}

template <typename R, typename F, typename Future>
continuable_future<R, inline_executor> inline_executor::then_execute(
    F&& func, Future&& pred) const {
  auto [predFactory, predRequest] = std::move(pred).ToFactory();
  using Factory = PromiseFactory<R>;
  auto& factory = Factory::GetInstance();
  uint64_t req = factory.CreateRequest();
  if constexpr (std::is_void_v<typename Future::value_type>) {
    if constexpr (std::is_void_v<R>) {
      predFactory->SetThen(predRequest, &factory, req, nullptr,
                           [func](void* f, uint64_t r, std::shared_ptr<void>) {
                             func();
                             static_cast<Factory*>(f)->SetValue(r);
                           });
    } else {
      predFactory->SetThen(predRequest, &factory, req, nullptr,
                           [func](void* f, uint64_t r, std::shared_ptr<void>) {
                             static_cast<Factory*>(f)->SetValue(r, func());
                           });
    }
  } else {
    if constexpr (std::is_void_v<R>) {
      predFactory->SetThen(predRequest, &factory, req, nullptr,
                           [func](void* f, uint64_t r, std::shared_ptr<void>,
                                  typename Future::value_type value) {
                             func(std::move(value));
                             static_cast<Factory*>(f)->SetValue(r);
                           });
    } else {
      predFactory->SetThen(predRequest, &factory, req, nullptr,
                           [func](void* f, uint64_t r, std::shared_ptr<void>,
                                  typename Future::value_type value) {
                             static_cast<Factory*>(f)->SetValue(
                                 r, func(std::move(value)));
                           });
    }
  }
  return factory.CreateContinuableFuture(req, *this);
}

}  // namespace wpi

#endif  // WPIUTIL_WPI_FUTURE_INL_
