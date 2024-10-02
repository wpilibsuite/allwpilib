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
inline constexpr units::meter_t kWheelDiameter = 6_in;
inline constexpr double kEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    ((kWheelDiameter * std::numbers::pi) / kEncoderCPR).value();

// These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
// These values MUST be determined either experimentally or theoretically for
// *your* robot's drive. The SysId tool provides a convenient method for
// obtaining feedback and feedforward values for your robot.
inline constexpr double kTurnP = 1;
inline constexpr double kTurnI = 0;
inline constexpr double kTurnD = 0;

inline constexpr auto kTurnTolerance = 5_deg;
inline constexpr auto kTurnRateTolerance = 10_deg_per_s;

inline constexpr auto kMaxTurnRate = 100_deg_per_s;
inline constexpr auto kMaxTurnAcceleration = 300_deg_per_s / 1_s;

inline constexpr auto ks = 1_V;
inline constexpr auto kv = 0.8_V * 1_s / 1_deg;
inline constexpr auto ka = 0.15_V * 1_s * 1_s / 1_deg;
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
// Distance units will be rotations
inline constexpr double kEncoderDistancePerPulse = 1.0 / kEncoderCPR;

inline constexpr int kShooterMotorPort = 4;
inline constexpr int kFeederMotorPort = 5;

inline constexpr auto kShooterFree = 5300_tps;
inline constexpr auto kShooterTarget = 4000_tps;
inline constexpr auto kShooterTolerance = 50_tps;

// These are not real PID gains, and will have to be tuned for your specific
// robot.
inline constexpr double kP = 1;

inline constexpr units::volt_t kS = 0.05_V;
// Should have value 12V at free speed
inline constexpr auto kV = 12_V / kShooterFree;

inline constexpr double kFeederSpeed = 0.5;
}  // namespace ShooterConstants

namespace OIConstants {
inline constexpr int kDriverControllerPort = 0;
}  // namespace OIConstants

namespace AutoConstants {
constexpr units::second_t kTimeout = 3_s;
constexpr units::meter_t kDriveDistance = 2_m;
constexpr double kDriveSpeed = 0.5;
}  // namespace AutoConstants
