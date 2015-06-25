#pragma once

#include <cstdint>
#include <condition_variable>

#include "HAL/cpp/priority_mutex.h"

class Semaphore {
 public:
  explicit Semaphore(uint32_t count = 0);
  Semaphore(Semaphore&&);
  Semaphore& operator=(Semaphore&&);

  void give();
  void take();

  // @return true if semaphore was locked successfully. false if not.
  bool tryTake();

  static const int32_t kNoWait = 0;
  static const int32_t kWaitForever = -1;

  static const uint32_t kEmpty = 0;
  static const uint32_t kFull = 1;

 private:
  priority_mutex m_mutex;
  std::condition_variable_any m_condition;
  uint32_t m_count = 0;
};
