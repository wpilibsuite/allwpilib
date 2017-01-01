/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>
#include <thread>

#include "ErrorBase.h"
#include "HAL/HAL.h"
#include "support/deprecated.h"

namespace frc {

/**
 * Wrapper class around std::thread that allows changing thread priority
 */
class WPI_DEPRECATED(
    "Task API scheduled for removal in 2018. Replace with std::thread, and use "
    "Threads API for setting priority") Task : public ErrorBase {
 public:
  static const int kDefaultPriority = 60;

  Task() = default;
  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;
  Task& operator=(Task&& task);

  template <class Function, class... Args>
  Task(const std::string& name, Function&& function, Args&&... args);

  virtual ~Task();

  bool joinable() const noexcept;
  void join();
  void detach();
  std::thread::id get_id() const noexcept;
  std::thread::native_handle_type native_handle();

  bool Verify();

  int GetPriority();

  bool SetPriority(int priority);

  std::string GetName() const;

 private:
  std::thread m_thread;
  std::string m_taskName;

  typedef int32_t TASK_STATUS;

  static constexpr int32_t TASK_OK = 0;
  static constexpr int32_t TASK_ERROR = -1;
  static constexpr int32_t TaskLib_ILLEGAL_PRIORITY = 22;  // 22 is EINVAL

  bool HandleError(TASK_STATUS results);
  TASK_STATUS VerifyTaskId();
  TASK_STATUS GetTaskPriority(int32_t* priority);
  TASK_STATUS SetTaskPriority(int32_t priority);
};

}  // namespace frc

#include "Task.inc"
