// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <units/acceleration.h>
#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>
#include <units/voltage.h>

/**
 * The Constants header provides a convenient place for teams to hold robot-wide
 * numerical or bool constants.  This should not be used for any other purpose.
 *
 * It is generally a good idea to place constants into subsystem- or
 * command-specific namespaces within this header, which can then be used where
 * they are needed.
 */

namespace DriveConstants {
inline constexpr units::second_t DT{0.02};
inline constexpr int LEFT_MOTOR_1_PORT = 0;
inline constexpr int LEFT_MOTOR_2_PORT = 1;
inline constexpr int RIGHT_MOTOR_1_PORT = 2;
inline constexpr int RIGHT_MOTOR_2_PORT = 3;

// These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
// These characterization values MUST be determined either experimentally or
// theoretically for *your* robot's drive. The SysId tool provides a convenient
// method for obtaining these values for your robot.
inline constexpr auto ks = 1_V;
inline constexpr auto kv = 0.8_V * 1_s / 1_m;
inline constexpr auto ka = 0.15_V * 1_s * 1_s / 1_m;

inline constexpr double kp = 1;

inline constexpr auto MAX_SPEED = 3_mps;
inline constexpr auto MAX_ACCELERATION = 3_mps_sq;

}  // namespace DriveConstants

namespace OIConstants {
inline constexpr int DRIVER_CONTROLLER_PORT = 0;
}  // namespace OIConstants
