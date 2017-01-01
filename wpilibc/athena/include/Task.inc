/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <atomic>
#include <iostream>
#include <string>
#include <utility>

namespace frc {

/**
 * Create and launch a task.
 *
 * @param name The name of the task. "FRC_" will be prepended to the task name.
 * @param function The address of the function to run as the new task.
 * @param args A parameter pack of arguments to pass to the function.
 */
template <class Function, class... Args>
Task::Task(const std::string& name, Function&& function, Args&&... args) {
  m_taskName = "FRC_";
  m_taskName += name;

  std::cout << "[HAL] Starting task " << m_taskName << "..." << std::endl;

  m_thread = std::thread(std::forward<std::decay_t<Function>>(function),
                         std::forward<Args>(args)...);
  // TODO: lvuser does not currently have permissions to set the priority.
  // SetPriority(kDefaultPriority);

  static std::atomic<int32_t> instances{0};
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_Task, instances, 0,
             m_taskName.c_str());
}

}  // namespace frc
