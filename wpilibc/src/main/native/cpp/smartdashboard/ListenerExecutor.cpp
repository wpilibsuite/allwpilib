// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/ListenerExecutor.h"

#include <utility>

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
