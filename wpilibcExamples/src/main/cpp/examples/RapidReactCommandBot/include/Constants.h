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
constexpr int kLeftMotor1Port = 0;
constexpr int kLeftMotor2Port = 1;
constexpr int kRightMotor1Port = 2;
constexpr int kRightMotor2Port = 3;

constexpr int kLeftEncoderPorts[]{0, 1};
constexpr int kRightEncoderPorts[]{2, 3};
constexpr bool kLeftEncoderReversed = false;
constexpr bool kRightEncoderReversed = true;

constexpr double kEncoderCPR = 1024;
constexpr units::meter_t kWheelDiameter = 6.0_in;
constexpr double kEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    ((kWheelDiameter * std::numbers::pi) / kEncoderCPR).value();

}  // namespace DriveConstants

namespace IntakeConstants {
constexpr int kMotorPort = 6;
constexpr int kSolenoidPorts[]{0, 1};
}  // namespace IntakeConstants

namespace StorageConstants {
constexpr int kMotorPort = 7;
constexpr int kBallSensorPort = 6;
}  // namespace StorageConstants

namespace ShooterConstants {
constexpr int kEncoderPorts[]{4, 5};
constexpr bool kEncoderReversed = false;
constexpr double kEncoderCPR = 1024;
constexpr double kEncoderDistancePerPulse =
    // Distance units will be rotations
    1.0 / kEncoderCPR;

constexpr int kShooterMotorPort = 4;
constexpr int kFeederMotorPort = 5;

constexpr auto kShooterFree = 5300_tps;
constexpr auto kShooterTarget = 4000_tps;
constexpr auto kShooterTolerance = 50_tps;

// These are not real PID gains, and will have to be tuned for your specific
// robot.
constexpr double kP = 1;

constexpr units::volt_t kS = 0.05_V;
constexpr auto kV =
    // Should have value 12V at free speed...
    12.0_V / kShooterFree;

constexpr double kFeederSpeed = 0.5;
}  // namespace ShooterConstants

namespace OIConstants {
constexpr int kDriverControllerPort = 0;
}  // namespace OIConstants

namespace AutoConstants {
constexpr units::second_t kTimeout = 3.0_s;
constexpr units::meter_t kDriveDistance = 2.0_m;
constexpr double kDriveSpeed = 0.5;
}  // namespace AutoConstants
