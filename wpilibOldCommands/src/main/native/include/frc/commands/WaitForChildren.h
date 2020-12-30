// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/Twine.h>

#include "frc/commands/Command.h"

namespace frc {

class WaitForChildren : public Command {
 public:
  explicit WaitForChildren(double timeout);
  WaitForChildren(const wpi::Twine& name, double timeout);
  ~WaitForChildren() override = default;

  WaitForChildren(WaitForChildren&&) = default;
  WaitForChildren& operator=(WaitForChildren&&) = default;

 protected:
  bool IsFinished() override;
};

}  // namespace frc
