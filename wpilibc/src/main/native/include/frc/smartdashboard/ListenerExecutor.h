/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
