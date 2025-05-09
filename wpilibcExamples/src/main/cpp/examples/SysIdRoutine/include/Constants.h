// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <numbers>

#include <units/angle.h>
#include <units/angular_acceleration.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/time.h>
#include <units/voltage.h>

namespace constants {
namespace drive {
inline constexpr int LEFT_MOTOR_1_PORT = 0;
inline constexpr int LEFT_MOTOR_2_PORT = 1;
inline constexpr int RIGHT_MOTOR_1_PORT = 2;
inline constexpr int RIGHT_MOTOR_2_PORT = 3;

inline constexpr std::array<int, 2> LEFT_ENCODER_PORTS = {0, 1};
inline constexpr std::array<int, 2> RIGHT_ENCODER_PORTS = {2, 3};
inline constexpr bool LEFT_ENCODER_REVERSED = false;
inline constexpr bool RIGHT_ENCODER_REVERSED = true;

inline constexpr int ENCODER_CPR = 1024;
inline constexpr units::meter_t WHEEL_DIAMETER = 6_in;
inline constexpr units::meter_t ENCODER_DISTANCE_PER_PULSE =
    (WHEEL_DIAMETER * std::numbers::pi) / static_cast<double>(ENCODER_CPR);
}  // namespace drive

namespace shooter {

using kv_unit =
    units::compound_unit<units::compound_unit<units::volts, units::seconds>,
                         units::inverse<units::turns>>;
using kv_unit_t = units::unit_t<kv_unit>;

using ka_unit =
    units::compound_unit<units::volts,
                         units::inverse<units::turns_per_second_squared>>;
using ka_unit_t = units::unit_t<ka_unit>;

inline constexpr std::array<int, 2> ENCODER_PORTS = {4, 5};
inline constexpr bool ENCODER_REVERSED = false;
inline constexpr int ENCODER_CPR = 1024;
inline constexpr units::turn_t ENCODER_DISTANCE_PER_PULSE =
    1_tr / static_cast<double>(ENCODER_CPR);

inline constexpr int SHOOTER_MOTOR_PORT = 4;
inline constexpr int FEEDER_MOTOR_PORT = 5;

inline constexpr units::turns_per_second_t SHOOTER_FREE_SPEED = 5300_tps;
inline constexpr units::turns_per_second_t SHOOTER_TARGET_SPEED = 4000_tps;
inline constexpr units::turns_per_second_t SHOOTER_TOLERANCE = 50_tps;

inline constexpr double kP = 1.0;

inline constexpr units::volt_t S = 0.05_V;
inline constexpr kv_unit_t kV = 12_V / SHOOTER_FREE_SPEED;
inline constexpr ka_unit_t kA = 0_V * 1_s * 1_s / 1_tr;

inline constexpr double FEEDER_SPEED = 0.5;
}  // namespace shooter

namespace intake {
inline constexpr int MOTOR_PORT = 6;
inline constexpr std::array<int, 2> SOLENOID_PORTS = {2, 3};
}  // namespace intake

namespace storage {
inline constexpr int MOTOR_PORT = 7;
inline constexpr int BALL_SENSOR_PORT = 6;
}  // namespace storage

namespace autonomous {
inline constexpr units::second_t TIMEOUT = 3_s;
inline constexpr units::meter_t DRIVE_DISTANCE = 2_m;
inline constexpr double DRIVE_SPEED = 0.5;
}  // namespace autonomous

namespace oi {
inline constexpr int DRIVER_CONTROLLER_PORT = 0;
}  // namespace oi
}  // namespace constants
