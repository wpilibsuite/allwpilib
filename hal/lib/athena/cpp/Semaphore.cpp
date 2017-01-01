/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/cpp/Semaphore.h"

Semaphore::Semaphore(int32_t count) { m_count = count; }

void Semaphore::give() {
  std::lock_guard<priority_mutex> lock(m_mutex);
  ++m_count;
  m_condition.notify_one();
}

void Semaphore::take() {
  std::unique_lock<priority_mutex> lock(m_mutex);
  m_condition.wait(lock, [this] { return m_count; });
  --m_count;
}

bool Semaphore::tryTake() {
  std::lock_guard<priority_mutex> lock(m_mutex);
  if (m_count) {
    --m_count;
    return true;
  } else {
    return false;
  }
}
