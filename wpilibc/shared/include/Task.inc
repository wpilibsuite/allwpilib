#include "HAL/HAL.hpp"
#include <atomic>

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
  //TODO: lvuser does not currently have permissions to set the priority.
  //SetPriority(kDefaultPriority);

  static std::atomic<int32_t> instances{0};
  instances++;
  HALReport(HALUsageReporting::kResourceType_Task, instances, 0, m_taskName.c_str());
}
