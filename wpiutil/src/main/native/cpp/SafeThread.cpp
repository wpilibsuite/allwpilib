// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/SafeThread.h"

using namespace wpi;

void SafeThread::Stop() {
  m_active = false;
  m_cond.notify_all();
}

void SafeThreadEvent::Stop() {
  m_active = false;
  m_stopEvent.Set();
}

detail::SafeThreadProxyBase::SafeThreadProxyBase(
    std::shared_ptr<SafeThreadBase> thr)
    : m_thread(std::move(thr)) {
  if (!m_thread) {
    return;
  }
  m_lock = std::unique_lock<wpi::mutex>(m_thread->m_mutex);
  if (!m_thread->m_active) {
    m_lock.unlock();
    m_thread = nullptr;
    return;
  }
}

detail::SafeThreadOwnerBase::~SafeThreadOwnerBase() {
  if (m_joinAtExit) {
    Join();
  } else {
    Stop();
  }
}

void detail::SafeThreadOwnerBase::Start(std::shared_ptr<SafeThreadBase> thr) {
  std::scoped_lock lock(m_mutex);
  if (auto thr = m_thread.lock()) {
    return;
  }
  m_stdThread = std::thread([=] { thr->Main(); });
  thr->m_threadId = m_stdThread.get_id();
  m_thread = thr;
}

void detail::SafeThreadOwnerBase::Stop() {
  std::scoped_lock lock(m_mutex);
  if (auto thr = m_thread.lock()) {
    thr->Stop();
    m_thread.reset();
  }
  if (m_stdThread.joinable()) {
    m_stdThread.detach();
  }
}

void detail::SafeThreadOwnerBase::Join() {
  std::unique_lock lock(m_mutex);
  if (auto thr = m_thread.lock()) {
    auto stdThread = std::move(m_stdThread);
    m_thread.reset();
    lock.unlock();
    thr->Stop();
    stdThread.join();
  } else if (m_stdThread.joinable()) {
    m_stdThread.detach();
  }
}

void detail::swap(SafeThreadOwnerBase& lhs, SafeThreadOwnerBase& rhs) noexcept {
  using std::swap;
  if (&lhs == &rhs) {
    return;
  }
  std::scoped_lock lock(lhs.m_mutex, rhs.m_mutex);
  std::swap(lhs.m_stdThread, rhs.m_stdThread);
  std::swap(lhs.m_thread, rhs.m_thread);
}

detail::SafeThreadOwnerBase::operator bool() const {
  std::scoped_lock lock(m_mutex);
  return !m_thread.expired();
}

std::thread::native_handle_type
detail::SafeThreadOwnerBase::GetNativeThreadHandle() {
  std::scoped_lock lock(m_mutex);
  return m_stdThread.native_handle();
}

std::shared_ptr<SafeThreadBase>
detail::SafeThreadOwnerBase::GetThreadSharedPtr() const {
  std::scoped_lock lock(m_mutex);
  return m_thread.lock();
}
