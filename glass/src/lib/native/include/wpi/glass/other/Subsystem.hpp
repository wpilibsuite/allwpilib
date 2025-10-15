// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/Model.h"

namespace glass {
class DataSource;
class SubsystemModel : public Model {
 public:
  virtual const char* GetName() const = 0;
  virtual const char* GetDefaultCommand() const = 0;
  virtual const char* GetCurrentCommand() const = 0;
};
void DisplaySubsystem(SubsystemModel* m);
}  // namespace glass
