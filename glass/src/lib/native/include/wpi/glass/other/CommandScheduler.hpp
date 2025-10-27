// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>

#include "glass/Model.h"

namespace glass {
class DataSource;
class CommandSchedulerModel : public Model {
 public:
  virtual const char* GetName() const = 0;
  virtual const std::vector<std::string>& GetCurrentCommands() = 0;
  virtual void CancelCommand(size_t index) = 0;
};
void DisplayCommandScheduler(CommandSchedulerModel* m);
}  // namespace glass
