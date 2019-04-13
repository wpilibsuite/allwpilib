/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/future.h"

namespace wpi {
namespace detail {

PromiseFactoryBase::~PromiseFactoryBase() {
  m_active = false;
  m_resultCv.notify_all();  // wake up any waiters
}

void PromiseFactoryBase::IgnoreResult(uint64_t request) {
  std::unique_lock<wpi::mutex> lock(m_resultMutex);
  EraseRequest(request);
}

uint64_t PromiseFactoryBase::CreateRequest() {
  std::unique_lock<wpi::mutex> lock(m_resultMutex);
  uint64_t req = ++m_uid;
  m_requests.push_back(req);
  return req;
}

bool PromiseFactoryBase::EraseRequest(uint64_t request) {
  if (request == 0) return false;
  auto it = std::find_if(m_requests.begin(), m_requests.end(),
                         [=](auto r) { return r == request; });
  if (it == m_requests.end()) return false;  // no waiters
  m_requests.erase(it);
  return true;
}

}  // namespace detail

future<void> PromiseFactory<void>::MakeReadyFuture() {
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  uint64_t req = CreateErasedRequest();
  m_results.emplace_back(req);
  return future<void>{this, req};
}

void PromiseFactory<void>::SetValue(uint64_t request) {
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  if (!EraseRequest(request)) return;
  auto it = std::find_if(m_thens.begin(), m_thens.end(),
                         [=](const auto& x) { return x.request == request; });
  if (it != m_thens.end()) {
    uint64_t outRequest = it->outRequest;
    ThenFunction func = std::move(it->func);
    m_thens.erase(it);
    lock.unlock();
    return func(outRequest);
  }
  m_results.emplace_back(request);
  Notify();
}

void PromiseFactory<void>::SetThen(uint64_t request, uint64_t outRequest,
                                   ThenFunction func) {
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  auto it = std::find_if(m_results.begin(), m_results.end(),
                         [=](const auto& r) { return r == request; });
  if (it != m_results.end()) {
    m_results.erase(it);
    lock.unlock();
    return func(outRequest);
  }
  m_thens.emplace_back(request, outRequest, func);
}

bool PromiseFactory<void>::IsReady(uint64_t request) noexcept {
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  auto it = std::find_if(m_results.begin(), m_results.end(),
                         [=](const auto& r) { return r == request; });
  return it != m_results.end();
}

void PromiseFactory<void>::GetResult(uint64_t request) {
  // wait for response
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  while (IsActive()) {
    // Did we get a response to *our* request?
    auto it = std::find_if(m_results.begin(), m_results.end(),
                           [=](const auto& r) { return r == request; });
    if (it != m_results.end()) {
      // Yes, remove it from the vector and we're done.
      m_results.erase(it);
      return;
    }
    // No, keep waiting for a response
    Wait(lock);
  }
}

void PromiseFactory<void>::WaitResult(uint64_t request) {
  // wait for response
  std::unique_lock<wpi::mutex> lock(GetResultMutex());
  while (IsActive()) {
    // Did we get a response to *our* request?
    auto it = std::find_if(m_results.begin(), m_results.end(),
                           [=](const auto& r) { return r == request; });
    if (it != m_results.end()) return;
    // No, keep waiting for a response
    Wait(lock);
  }
}

PromiseFactory<void>& PromiseFactory<void>::GetInstance() {
  static PromiseFactory<void> inst;
  return inst;
}

}  // namespace wpi
