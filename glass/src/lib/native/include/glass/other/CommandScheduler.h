/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>
#include <vector>

#include <wpi/STLExtras.h>
#include <wpi/StringRef.h>

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
