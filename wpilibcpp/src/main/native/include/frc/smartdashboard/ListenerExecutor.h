// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

#include <wpi/mutex.h>

namespace frc::detail {
/**
 * An executor for running listener tasks posted by Sendable listeners
 * synchronously from the main application thread.
 *
 * @see Sendable
 */
class ListenerExecutor {
 public:
  /**
   * Posts a task to the executor to be run synchronously from the main thread.
   *
   * @param task The task to run synchronously from the main thread.
   */
  void Execute(std::function<void()> task);

  /**
   * Runs all posted tasks.  Called periodically from main thread.
   */
  void RunListenerTasks();

 private:
  std::vector<std::function<void()>> m_tasks;
  std::vector<std::function<void()>> m_runningTasks;
  wpi::mutex m_lock;
};
}  // namespace frc::detail
