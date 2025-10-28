// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

/**
 * The Constants header provides a convenient place for teams to hold robot-wide
 * numerical or bool constants.  This should not be used for any other purpose.
 *
 * It is generally a good idea to place constants into subsystem- or
 * command-specific namespaces within this header, which can then be used where
 * they are needed.
 */

namespace IntakeConstants {
inline constexpr int kMotorPort = 1;

inline constexpr int kPistonFwdChannel = 0;
inline constexpr int kPistonRevChannel = 1;
inline constexpr double kIntakeSpeed = 0.5;
}  // namespace IntakeConstants

namespace OperatorConstants {
inline constexpr int kJoystickIndex = 0;
}  // namespace OperatorConstants
