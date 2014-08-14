/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "ErrorBase.h"
#include "HAL/Task.hpp"
#include <iostream>
#include <string>
#include <thread>

/**
 * WPI task is a wrapper for the native Task object.
 * All WPILib tasks are managed by a static task manager for simplified cleanup.
 **/
class Task : public ErrorBase {
 public:
  static const uint32_t kDefaultPriority = 60;

  Task() = default;
  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;
  Task& operator=(Task&& task);

  template <class Function, class... Args>
  Task(const char* name, Function&& function, Args&&... args);

  virtual ~Task();

  bool joinable() const noexcept;
  void join();
  void detach();
  std::thread::id get_id() const noexcept;
  std::thread::native_handle_type native_handle();

  bool Verify();

  int32_t GetPriority();

  /**
   * @param priority The priority at which the internal thread should run.
   *                 Must be within range 1 to 99 inclusive for pthreads.
   */
  bool SetPriority(int32_t priority);

  std::string GetName() const;

 private:
  std::thread m_thread;
  std::string m_taskName;
  bool HandleError(STATUS results);
};

#include "Task.inc"
