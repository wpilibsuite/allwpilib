/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SafeThread.h"

using namespace nt;

void detail::SafeThreadOwnerBase::Start(SafeThread* thr) {
  SafeThread* curthr = nullptr;
  SafeThread* newthr = thr;
  if (!m_thread.compare_exchange_strong(curthr, newthr)) {
    delete newthr;
    return;
  }
  std::thread([=]() {
    newthr->Main();
    delete newthr;
  }).detach();
}

void detail::SafeThreadOwnerBase::Stop() {
  SafeThread* thr = m_thread.exchange(nullptr);
  if (!thr) return;
  std::lock_guard<std::mutex> lock(thr->m_mutex);
  thr->m_active = false;
  thr->m_cond.notify_one();
}
