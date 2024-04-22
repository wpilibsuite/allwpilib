// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc2/command/Command.h"

namespace frc2 {
/**
 * A Sendable base class for Commands.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @deprecated All functionality provided by CommandBase has been merged into
 * Command. Use Command instead.
 */
class [[deprecated("Use Command instead")]] CommandBase : public Command {
 protected:
  [[deprecated("Use Command instead")]]
  CommandBase();
};
}  // namespace frc2
