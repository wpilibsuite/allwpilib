// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/time.h>
#include <units/voltage.h>

namespace DriveConstants {
inline constexpr int kLeftMotor1Port = 0;
inline constexpr int kLeftMotor2Port = 1;
inline constexpr int kRightMotor1Port = 2;
inline constexpr int kRightMotor2Port = 3;

inline constexpr int kLeftEncoderPorts[]{0, 1};
inline constexpr int kRightEncoderPorts[]{2, 3};
inline constexpr bool kLeftEncoderReversed = false;
inline constexpr bool kRightEncoderReversed = true;

inline constexpr double kEncoderCPR = 1024;
inline constexpr units::meter_t kWheelDiameter = 6.0_in;
inline constexpr double kEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    ((kWheelDiameter * std::numbers::pi) / kEncoderCPR).value();

}  // namespace DriveConstants

namespace IntakeConstants {
inline constexpr int kMotorPort = 6;
inline constexpr int kSolenoidPorts[]{0, 1};
}  // namespace IntakeConstants

namespace StorageConstants {
inline constexpr int kMotorPort = 7;
inline constexpr int kBallSensorPort = 6;
}  // namespace StorageConstants

namespace ShooterConstants {
inline constexpr int kEncoderPorts[]{4, 5};
inline constexpr bool kEncoderReversed = false;
inline constexpr double kEncoderCPR = 1024;
inline constexpr double kEncoderDistancePerPulse =
    // Distance units will be rotations
    1.0 / kEncoderCPR;

inline constexpr int kShooterMotorPort = 4;
inline constexpr int kFeederMotorPort = 5;

inline constexpr auto kShooterFree = 5300_tps;
inline constexpr auto kShooterTarget = 4000_tps;
inline constexpr auto kShooterTolerance = 50_tps;

// These are not real PID gains, and will have to be tuned for your specific
// robot.
inline constexpr double kP = 1;

inline constexpr units::volt_t kS = 0.05_V;
inline constexpr auto kV =
    // Should have value 12V at free speed...
    12.0_V / kShooterFree;

inline constexpr double kFeederSpeed = 0.5;
}  // namespace ShooterConstants

namespace OIConstants {
inline constexpr int kDriverControllerPort = 0;
}  // namespace OIConstants

namespace AutoConstants {
constexpr units::second_t kTimeout = 3.0_s;
constexpr units::meter_t kDriveDistance = 2.0_m;
constexpr double kDriveSpeed = 0.5;
}  // namespace AutoConstants
