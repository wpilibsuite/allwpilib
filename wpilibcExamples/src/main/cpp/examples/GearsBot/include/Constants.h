// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

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

/**
 * The Constants header provides a convenient place for teams to hold robot-wide
 * numerical or bool constants.  This should not be used for any other purpose.
 *
 * It is generally a good idea to place constants into subsystem- or
 * command-specific namespaces within this header, which can then be used where
 * they are needed.
 */

namespace DriveConstants {
inline constexpr int kLeftMotorPort1 = 0;
inline constexpr int kLeftMotorPort2 = 1;

inline constexpr int kRightMotorPort1 = 2;
inline constexpr int kRightMotorPort2 = 3;

inline constexpr int kLeftEncoderPorts[]{0, 1};
inline constexpr int kRightEncoderPorts[]{2, 3};
inline constexpr bool kLeftEncoderReversed = false;
inline constexpr bool kRightEncoderReversed = true;

inline constexpr int kRangeFinderPort = 6;
inline constexpr int kAnalogGyroPort = 1;

inline constexpr int kEncoderCPR = 1024;
inline constexpr int kWheelDiameterMeters = 0.15;
inline constexpr double kEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (kWheelDiameterMeters * std::numbers::pi) /
    static_cast<double>(kEncoderCPR);
}  // namespace DriveConstants

namespace ClawConstants {
inline constexpr int kMotorPort = 7;
inline constexpr int kContactPort = 5;
}  // namespace ClawConstants

namespace WristConstants {
inline constexpr int kMotorPort = 6;

// these PID constants are not real, and will need to be tuned for your robot
inline constexpr double kP = 0.1;
inline constexpr double kI = 0.0;
inline constexpr double kD = 0.0;

inline constexpr double kTolerance = 2.5;

inline constexpr int kPotentiometerPort = 3;
}  // namespace WristConstants

namespace ElevatorConstants {
inline constexpr int kMotorPort = 5;
inline constexpr int kPotentiometerPort = 2;

// these PID constants are not real, and will need to be tuned for your robot
inline constexpr double kP_real = 4;
inline constexpr double kI_real = 0.007;

inline constexpr double kI_simulation = 18;
inline constexpr double kP_simulation = 0.2;

inline constexpr double kD = 0.0;

inline constexpr double kTolerance = 0.005;
}  // namespace ElevatorConstants

namespace AutoConstants {

inline constexpr auto kDistToBox1 = 0.1_m;
inline constexpr auto kDistToBox2 = 0.6_m;

inline constexpr auto kWristSetpoint = -45.0_deg;

}  // namespace AutoConstants

namespace DriveStraightConstants {
// these PID constants are not real, and will need to be tuned for your robot
inline constexpr double kP = 4.0;
inline constexpr double kI = 0.0;
inline constexpr double kD = 0.0;

inline constexpr double kTolerance = 0.01;
}  // namespace DriveStraightConstants

namespace BoxAlignConstants {
inline constexpr double kP = 2.0;
inline constexpr double kI = 0.0;
inline constexpr double kD = 0.0;

inline constexpr double kTolerance = 0.01;

}  // namespace BoxAlignConstants

namespace OIConstants {
inline constexpr int kDriverControllerPort = 0;
}  // namespace OIConstants

namespace Positions {
namespace Pickup {
inline constexpr auto kWristSetpoint = -45_deg;
inline constexpr auto kElevatorSetpoint = 0.25;
}  // namespace Pickup
namespace Place {
inline constexpr auto kWristSetpoint = 0.0_deg;
inline constexpr auto kElevatorSetpoint = 0.25;
}  // namespace Place
namespace PrepareToPickup {
inline constexpr auto kWristSetpoint = 0.0_deg;
inline constexpr auto kElevatorSetpoint = 0.0;
}  // namespace PrepareToPickup
}  // namespace Positions
