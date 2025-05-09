// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

/**
 * The Constants header provides a convenient place for teams to hold robot-wide
 * numerical or bool constants.  This should not be used for any other purpose.
 *
 * It is generally a good idea to place constants into subsystem- or
 * command-specific namespaces within this header, which can then be used where
 * they are needed.
 */

namespace DriveConstants {
inline constexpr int LEFT_MOTOR_1_PORT = 0;
inline constexpr int LEFT_MOTOR_2_PORT = 1;
inline constexpr int RIGHT_MOTOR_1_PORT = 2;
inline constexpr int RIGHT_MOTOR_2_PORT = 3;

inline constexpr int LEFT_ENCODER_PORTS[]{0, 1};
inline constexpr int RIGHT_ENCODER_PORTS[]{2, 3};
inline constexpr bool LEFT_ENCODER_REVERSED = false;
inline constexpr bool RIGHT_ENCODER_REVERSED = true;

inline constexpr int ENCODER_CPR = 1024;
inline constexpr double WHEEL_DIAMETER_INCHES = 6;
inline constexpr double ENCODER_DISTANCE_PER_PULSE =
    // Assumes the encoders are directly mounted on the wheel shafts
    (WHEEL_DIAMETER_INCHES * std::numbers::pi) /
    static_cast<double>(ENCODER_CPR);
}  // namespace DriveConstants

namespace HatchConstants {
inline constexpr int HATCH_SOLENOID_MODULE = 0;
inline constexpr int HATCH_SOLENOID_PORTS[]{0, 1};
}  // namespace HatchConstants

namespace AutoConstants {
inline constexpr double AUTO_DRIVE_DISTANCE_INCHES = 60;
inline constexpr double AUTO_BACKUP_DISTANCE_INCHES = 20;
inline constexpr double AUTO_DRIVE_SPEED = 0.5;
}  // namespace AutoConstants

namespace OIConstants {
inline constexpr int DRIVER_CONTROLLER_PORT = 0;
}  // namespace OIConstants
