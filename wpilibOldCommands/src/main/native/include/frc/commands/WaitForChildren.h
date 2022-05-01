// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <units/time.h>

#include "frc/commands/Command.h"

namespace frc {

/**
 * This command will only finish if whatever CommandGroup it is in has no active
 * children. If it is not a part of a CommandGroup, then it will finish
 * immediately. If it is itself an active child, then the CommandGroup will
 * never end.
 *
 * This class is useful for the situation where you want to allow anything
 * running in parallel to finish, before continuing in the main CommandGroup
 * sequence.
 *
 * This class is provided by the OldCommands VendorDep
 */
class WaitForChildren : public Command {
 public:
  explicit WaitForChildren(units::second_t timeout);
  WaitForChildren(std::string_view name, units::second_t timeout);
  ~WaitForChildren() override = default;

  WaitForChildren(WaitForChildren&&) = default;
  WaitForChildren& operator=(WaitForChildren&&) = default;

 protected:
  bool IsFinished() override;
};

}  // namespace frc
