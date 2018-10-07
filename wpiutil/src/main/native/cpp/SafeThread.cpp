/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/SafeThread.h"

using namespace wpi;

detail::SafeThreadProxyBase::SafeThreadProxyBase(
    std::shared_ptr<SafeThread> thr)
    : m_thread(std::move(thr)) {
  if (!m_thread) return;
  m_lock = std::unique_lock<wpi::mutex>(m_thread->m_mutex);
  if (!m_thread->m_active) {
    m_lock.unlock();
    m_thread = nullptr;
    return;
  }
}

detail::SafeThreadOwnerBase::~SafeThreadOwnerBase() {
  if (m_joinAtExit)
    Join();
  else
    Stop();
}

void detail::SafeThreadOwnerBase::Start(std::shared_ptr<SafeThread> thr) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  if (auto thr = m_thread.lock()) return;
  m_stdThread = std::thread([=] { thr->Main(); });
  m_thread = thr;
}

void detail::SafeThreadOwnerBase::Stop() {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  if (auto thr = m_thread.lock()) {
    thr->m_active = false;
    thr->m_cond.notify_all();
    m_thread.reset();
  }
  if (m_stdThread.joinable()) m_stdThread.detach();
}

void detail::SafeThreadOwnerBase::Join() {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (auto thr = m_thread.lock()) {
    auto stdThread = std::move(m_stdThread);
    m_thread.reset();
    lock.unlock();
    thr->m_active = false;
    thr->m_cond.notify_all();
    stdThread.join();
  } else if (m_stdThread.joinable()) {
    m_stdThread.detach();
  }
}

void detail::swap(SafeThreadOwnerBase& lhs, SafeThreadOwnerBase& rhs) noexcept {
  using std::swap;
  if (&lhs == &rhs) return;
  std::lock(lhs.m_mutex, rhs.m_mutex);
  std::lock_guard<wpi::mutex> lock_lhs(lhs.m_mutex, std::adopt_lock);
  std::lock_guard<wpi::mutex> lock_rhs(rhs.m_mutex, std::adopt_lock);
  std::swap(lhs.m_stdThread, rhs.m_stdThread);
  std::swap(lhs.m_thread, rhs.m_thread);
}

detail::SafeThreadOwnerBase::operator bool() const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  return !m_thread.expired();
}

std::thread::native_handle_type
detail::SafeThreadOwnerBase::GetNativeThreadHandle() {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  return m_stdThread.native_handle();
}

std::shared_ptr<SafeThread> detail::SafeThreadOwnerBase::GetThread() const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  return m_thread.lock();
}
