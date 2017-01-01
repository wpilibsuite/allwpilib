/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <condition_variable>

#include "HAL/cpp/priority_mutex.h"
#include "support/deprecated.h"

class WPI_DEPRECATED(
    "Semaphore scheduled for removal in 2018. Recommended to replace with a "
    "std::mutex and std::condition_variable") Semaphore {
 public:
  explicit Semaphore(int32_t count = 0);
  Semaphore(Semaphore&&);
  Semaphore& operator=(Semaphore&&);

  void give();
  void take();

  // @return true if semaphore was locked successfully. false if not.
  bool tryTake();

  static const int32_t kNoWait = 0;
  static const int32_t kWaitForever = -1;

  static const int32_t kEmpty = 0;
  static const int32_t kFull = 1;

 private:
  priority_mutex m_mutex;
  std::condition_variable_any m_condition;
  int32_t m_count = 0;
};
