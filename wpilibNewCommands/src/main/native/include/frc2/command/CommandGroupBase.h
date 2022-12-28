// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <vector>

#include <wpi/deprecated.h>

#include "frc2/command/CommandBase.h"

namespace frc2 {

/**
 * A base for CommandGroups.
 *
 * This class is provided by the NewCommands VendorDep
 * @deprecated This class is an empty abstraction. Inherit directly from
 * CommandBase.
 */
class CommandGroupBase : public CommandBase {
 public:
  /**
   * Adds the given commands to the command group.
   *
   * @param commands The commands to add.
   */
  virtual void AddCommands(
      std::vector<std::unique_ptr<Command>>&& commands) = 0;
};
}  // namespace frc2
