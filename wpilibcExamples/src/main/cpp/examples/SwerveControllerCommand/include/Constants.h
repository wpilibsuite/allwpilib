// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <frc/TimedRobot.h>
#include <frc/geometry/Translation2d.h>
#include <frc/kinematics/SwerveDriveKinematics.h>
#include <frc/trajectory/TrapezoidProfile.h>
#include <units/acceleration.h>
#include <units/angle.h>
#include <units/angular_acceleration.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>
#include <units/voltage.h>

#pragma once

/**
 * The Constants header provides a convenient place for teams to hold robot-wide
 * numerical or bool constants.  This should not be used for any other purpose.
 *
 * It is generally a good idea to place constants into subsystem- or
 * command-specific namespaces within this header, which can then be used where
 * they are needed.
 */

namespace DriveConstants {
inline constexpr int FRONT_LEFT_DRIVE_MOTOR_PORT = 0;
inline constexpr int REAR_LEFT_DRIVE_MOTOR_PORT = 2;
inline constexpr int FRONT_RIGHT_DRIVE_MOTOR_PORT = 4;
inline constexpr int REAR_RIGHT_DRIVE_MOTOR_PORT = 6;

inline constexpr int FRONT_LEFT_TURNING_MOTOR_PORT = 1;
inline constexpr int REAR_LEFT_TURNING_MOTOR_PORT = 3;
inline constexpr int FRONT_RIGHT_TURNING_MOTOR_PORT = 5;
inline constexpr int REAR_RIGHT_TURNING_MOTOR_PORT = 7;

inline constexpr int FRONT_LEFT_TURNING_ENCODER_PORTS[2]{0, 1};
inline constexpr int REAR_LEFT_TURNING_ENCODER_PORTS[2]{2, 3};
inline constexpr int FRONT_RIGHT_TURNING_ENCODER_PORTS[2]{4, 5};
inline constexpr int REAR_RIGHT_TURNING_ENCODER_PORTS[2]{6, 7};

inline constexpr bool FRONT_LEFT_TURNING_ENCODER_REVERSED = false;
inline constexpr bool REAR_LEFT_TURNING_ENCODER_REVERSED = true;
inline constexpr bool FRONT_RIGHT_TURNING_ENCODER_REVERSED = false;
inline constexpr bool REAR_RIGHT_TURNING_ENCODER_REVERSED = true;

inline constexpr int FRONT_LEFT_DRIVE_ENCODER_PORTS[2]{8, 9};
inline constexpr int REAR_LEFT_DRIVE_ENCODER_PORTS[2]{10, 11};
inline constexpr int FRONT_RIGHT_DRIVE_ENCODER_PORTS[2]{12, 13};
inline constexpr int REAR_RIGHT_DRIVE_ENCODER_PORTS[2]{14, 15};

inline constexpr bool FRONT_LEFT_DRIVE_ENCODER_REVERSED = false;
inline constexpr bool REAR_LEFT_DRIVE_ENCODER_REVERSED = true;
inline constexpr bool FRONT_RIGHT_DRIVE_ENCODER_REVERSED = false;
inline constexpr bool REAR_RIGHT_DRIVE_ENCODER_REVERSED = true;

// If you call DriveSubsystem::Drive with a different period make sure to update
// this.
inline constexpr units::second_t DRIVE_PERIOD = frc::TimedRobot::DEFAULT_PERIOD;

// These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
// These characterization values MUST be determined either experimentally or
// theoretically for *your* robot's drive. The SysId tool provides a convenient
// method for obtaining these values for your robot.
inline constexpr auto ks = 1_V;
inline constexpr auto kv = 0.8 * 1_V * 1_s / 1_m;
inline constexpr auto ka = 0.15 * 1_V * 1_s * 1_s / 1_m;

// Example value only - as above, this must be tuned for your drive!
inline constexpr double FRONT_LEFT_VEL_P = 0.5;
inline constexpr double REAR_LEFT_VEL_P = 0.5;
inline constexpr double FRONT_RIGHT_VEL_P = 0.5;
inline constexpr double REAR_RIGHT_VEL_P = 0.5;
}  // namespace DriveConstants

namespace ModuleConstants {
inline constexpr int ENCODER_CPR = 1024;
inline constexpr auto WHEEL_DIAMETER = 0.15_m;
inline constexpr double DRIVE_ENCODER_DISTANCE_PER_PULSE =
    // Assumes the encoders are directly mounted on the wheel shafts
    (WHEEL_DIAMETER.value() * std::numbers::pi) /
    static_cast<double>(ENCODER_CPR);

inline constexpr double TURNING_ENCODER_DISTANCE_PER_PULSE =
    // Assumes the encoders are directly mounted on the wheel shafts
    (std::numbers::pi * 2) / static_cast<double>(ENCODER_CPR);

inline constexpr double MODULE_TURNING_CONTROLLER_P = 1;
inline constexpr double MODULE_DRIVE_CONTROLLER_P = 1;
}  // namespace ModuleConstants

namespace AutoConstants {
inline constexpr auto MAX_SPEED = 3_mps;
inline constexpr auto MAX_ACCELERATION = 3_mps_sq;
inline constexpr auto MAX_ANGULAR_SPEED = 3.142_rad_per_s;
inline constexpr auto MAX_ANGULAR_ACCELERATION = 3.142_rad_per_s_sq;

inline constexpr double X_CONTROLLER_P = 0.5;
inline constexpr double Y_CONTROLLER_P = 0.5;
inline constexpr double THETA_CONTROLLER_P = 0.5;

//

extern const frc::TrapezoidProfile<units::radians>::Constraints
    THETA_CONTROLLER_CONSTRAINTS;

}  // namespace AutoConstants

namespace OIConstants {
inline constexpr int DRIVER_CONTROLLER_PORT = 0;
}  // namespace OIConstants
