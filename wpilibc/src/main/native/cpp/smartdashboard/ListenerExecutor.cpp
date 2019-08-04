/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/smartdashboard/ListenerExecutor.h"

using namespace frc::detail;

void ListenerExecutor::Execute(std::function<void()> task) {
  std::scoped_lock lock(m_lock);
  m_tasks.emplace_back(task);
}

void ListenerExecutor::RunListenerTasks() {
  // Locally copy tasks from internal list; minimizes blocking time
  {
    std::scoped_lock lock(m_lock);
    std::swap(m_tasks, m_runningTasks);
  }

  for (auto&& task : m_runningTasks) {
    task();
  }
  m_runningTasks.clear();
}
