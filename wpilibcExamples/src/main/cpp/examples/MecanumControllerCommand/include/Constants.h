// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <frc/geometry/Translation2d.h>
#include <frc/kinematics/MecanumDriveKinematics.h>
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
inline constexpr int kFrontLeftMotorPort = 0;
inline constexpr int kRearLeftMotorPort = 1;
inline constexpr int kFrontRightMotorPort = 2;
inline constexpr int kRearRightMotorPort = 3;

inline constexpr int kFrontLeftEncoderPorts[]{0, 1};
inline constexpr int kRearLeftEncoderPorts[]{2, 3};
inline constexpr int kFrontRightEncoderPorts[]{4, 5};
inline constexpr int kRearRightEncoderPorts[]{6, 7};

inline constexpr bool kFrontLeftEncoderReversed = false;
inline constexpr bool kRearLeftEncoderReversed = true;
inline constexpr bool kFrontRightEncoderReversed = false;
inline constexpr bool kRearRightEncoderReversed = true;

inline constexpr auto kTrackwidth =
    0.5_m;  // Distance between centers of right and left wheels on robot
inline constexpr auto kWheelBase =
    0.7_m;  // Distance between centers of front and back wheels on robot
extern const frc::MecanumDriveKinematics kDriveKinematics;

inline constexpr int kEncoderCPR = 1024;
inline constexpr auto kWheelDiameter = 0.15_m;
inline constexpr double kEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (kWheelDiameter.value() * std::numbers::pi) /
    static_cast<double>(kEncoderCPR);

// These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
// These characterization values MUST be determined either experimentally or
// theoretically for *your* robot's drive. The SysId tool provides a convenient
// method for obtaining these values for your robot.
inline constexpr auto ks = 1_V;
inline constexpr auto kv = 0.8 * 1_V * 1_s / 1_m;
inline constexpr auto ka = 0.15 * 1_V * 1_s * 1_s / 1_m;

// Example value only - as above, this must be tuned for your drive!
inline constexpr double kPFrontLeftVel = 0.5;
inline constexpr double kPRearLeftVel = 0.5;
inline constexpr double kPFrontRightVel = 0.5;
inline constexpr double kPRearRightVel = 0.5;
}  // namespace DriveConstants

namespace AutoConstants {
inline constexpr auto kMaxSpeed = 3_mps;
inline constexpr auto kMaxAcceleration = 3_mps_sq;
inline constexpr auto kMaxAngularSpeed = 3_rad_per_s;
inline constexpr auto kMaxAngularAcceleration = 3_rad_per_s_sq;

inline constexpr double kPXController = 0.5;
inline constexpr double kPYController = 0.5;
inline constexpr double kPThetaController = 0.5;

extern const frc::TrapezoidProfile<units::radians>::Constraints
    kThetaControllerConstraints;

}  // namespace AutoConstants

namespace OIConstants {
inline constexpr int kDriverControllerPort = 0;
}  // namespace OIConstants
