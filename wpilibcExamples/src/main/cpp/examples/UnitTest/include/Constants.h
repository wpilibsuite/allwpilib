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
inline constexpr int MOTOR_PORT = 1;

inline constexpr int PISTON_FWD_CHANNEL = 0;
inline constexpr int PISTON_REV_CHANNEL = 1;
inline constexpr double INTAKE_SPEED = 0.5;
}  // namespace IntakeConstants

namespace OperatorConstants {
inline constexpr int JOYSTICK_INDEX = 0;
}  // namespace OperatorConstants
