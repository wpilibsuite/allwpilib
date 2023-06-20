// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/deprecated.h>

#include "frc2/command/Subsystem.h"

namespace frc2 {
/**
 * A base for subsystems that handles registration in the constructor, and
 * provides a more intuitive method for setting the default command.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @deprecated All functionality provided by SubsystemBase has been merged into
 * Subsystem. Use Subsystem instead.
 */
class [[deprecated("Use Subsystem instead")]] SubsystemBase : public Subsystem {
 protected:
  WPI_DEPRECATED("Use Subsystem instead")
  SubsystemBase();
};
}  // namespace frc2
