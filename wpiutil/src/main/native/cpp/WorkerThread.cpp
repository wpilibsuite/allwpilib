/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/WorkerThread.h"

using namespace wpi;

void detail::WorkerThreadThread::Main() {
  std::vector<Entry> queue;
  while (m_active) {
    std::unique_lock<wpi::mutex> lock(m_mutex);
    m_cond.wait(lock, [&] { return !m_active || !m_queue.empty(); });
    if (!m_active) break;

    // don't want to hold the lock while executing the callbacks
    queue.swap(m_queue);
    lock.unlock();

    for (auto&& entry : queue) {
      if (!m_active) break;  // requests may be long-running
      entry.func(entry.factory, entry.req);
    }
    queue.clear();
  }
}
