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
inline constexpr int LEFT_MOTOR_1_PORT = 0;
inline constexpr int LEFT_MOTOR_2_PORT = 1;
inline constexpr int RIGHT_MOTOR_1_PORT = 2;
inline constexpr int RIGHT_MOTOR_2_PORT = 3;

inline constexpr int LEFT_ENCODER_PORTS[]{0, 1};
inline constexpr int RIGHT_ENCODER_PORTS[]{2, 3};
inline constexpr bool LEFT_ENCODER_REVERSED = false;
inline constexpr bool RIGHT_ENCODER_REVERSED = true;

inline constexpr double ENCODER_CPR = 1024;
inline constexpr units::meter_t WHEEL_DIAMETER = 6_in;
inline constexpr double ENCODER_DISTANCE_PER_PULSE =
    // Assumes the encoders are directly mounted on the wheel shafts
    ((WHEEL_DIAMETER * std::numbers::pi) / ENCODER_CPR).value();

// These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
// These values MUST be determined either experimentally or theoretically for
// *your* robot's drive. The SysId tool provides a convenient method for
// obtaining feedback and feedforward values for your robot.
inline constexpr double TURN_P = 1;
inline constexpr double TURN_I = 0;
inline constexpr double TURN_D = 0;

inline constexpr auto TURN_TOLERANCE = 5_deg;
inline constexpr auto TURN_RATE_TOLERANCE = 10_deg_per_s;

inline constexpr auto MAX_TURN_RATE = 100_deg_per_s;
inline constexpr auto MAX_TURN_ACCELERATION = 300_deg_per_s / 1_s;

inline constexpr auto ks = 1_V;
inline constexpr auto kv = 0.8_V * 1_s / 1_deg;
inline constexpr auto ka = 0.15_V * 1_s * 1_s / 1_deg;
}  // namespace DriveConstants

namespace IntakeConstants {
inline constexpr int MOTOR_PORT = 6;
inline constexpr int SOLENOID_PORTS[]{0, 1};
}  // namespace IntakeConstants

namespace StorageConstants {
inline constexpr int MOTOR_PORT = 7;
inline constexpr int BALL_SENSOR_PORT = 6;
}  // namespace StorageConstants

namespace ShooterConstants {
inline constexpr int ENCODER_PORTS[]{4, 5};
inline constexpr bool ENCODER_REVERSED = false;
inline constexpr double ENCODER_CPR = 1024;
// Distance units will be rotations
inline constexpr double ENCODER_DISTANCE_PER_PULSE = 1.0 / ENCODER_CPR;

inline constexpr int SHOOTER_MOTOR_PORT = 4;
inline constexpr int FEEDER_MOTOR_PORT = 5;

inline constexpr auto SHOOTER_FREE = 5300_tps;
inline constexpr auto SHOOTER_TARGET = 4000_tps;
inline constexpr auto SHOOTER_TOLERANCE = 50_tps;

// These are not real PID gains, and will have to be tuned for your specific
// robot.
inline constexpr double kP = 1;

inline constexpr units::volt_t S = 0.05_V;
// Should have value 12V at free speed
inline constexpr auto kV = 12_V / SHOOTER_FREE;

inline constexpr double FEEDER_SPEED = 0.5;
}  // namespace ShooterConstants

namespace OIConstants {
inline constexpr int DRIVER_CONTROLLER_PORT = 0;
}  // namespace OIConstants

namespace AutoConstants {
constexpr units::second_t TIMEOUT = 3_s;
constexpr units::meter_t DRIVE_DISTANCE = 2_m;
constexpr double DRIVE_SPEED = 0.5;
}  // namespace AutoConstants
