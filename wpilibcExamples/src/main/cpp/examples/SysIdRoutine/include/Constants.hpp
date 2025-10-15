// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <numbers>

#include <wpi/units/angle.hpp>
#include <wpi/units/angular_acceleration.hpp>
#include <wpi/units/angular_velocity.hpp>
#include <wpi/units/length.hpp>
#include <wpi/units/time.hpp>
#include <wpi/units/voltage.hpp>

namespace constants {
namespace drive {
inline constexpr int kLeftMotor1Port = 0;
inline constexpr int kLeftMotor2Port = 1;
inline constexpr int kRightMotor1Port = 2;
inline constexpr int kRightMotor2Port = 3;

inline constexpr std::array<int, 2> kLeftEncoderPorts = {0, 1};
inline constexpr std::array<int, 2> kRightEncoderPorts = {2, 3};
inline constexpr bool kLeftEncoderReversed = false;
inline constexpr bool kRightEncoderReversed = true;

inline constexpr int kEncoderCpr = 1024;
inline constexpr wpi::units::meter_t kWheelDiameter = 6_in;
inline constexpr wpi::units::meter_t kEncoderDistancePerPulse =
    (kWheelDiameter * std::numbers::pi) / static_cast<double>(kEncoderCpr);
}  // namespace drive

namespace shooter {

using kv_unit = wpi::units::compound_unit<
    wpi::units::compound_unit<wpi::units::volts, wpi::units::seconds>,
    wpi::units::inverse<wpi::units::turns>>;
using kv_unit_t = wpi::units::unit_t<kv_unit>;

using ka_unit = wpi::units::compound_unit<
    wpi::units::volts,
    wpi::units::inverse<wpi::units::turns_per_second_squared>>;
using ka_unit_t = wpi::units::unit_t<ka_unit>;

inline constexpr std::array<int, 2> kEncoderPorts = {4, 5};
inline constexpr bool kEncoderReversed = false;
inline constexpr int kEncoderCpr = 1024;
inline constexpr wpi::units::turn_t kEncoderDistancePerPulse =
    1_tr / static_cast<double>(kEncoderCpr);

inline constexpr int kShooterMotorPort = 4;
inline constexpr int kFeederMotorPort = 5;

inline constexpr wpi::units::turns_per_second_t kShooterFreeSpeed = 5300_tps;
inline constexpr wpi::units::turns_per_second_t kShooterTargetSpeed = 4000_tps;
inline constexpr wpi::units::turns_per_second_t kShooterTolerance = 50_tps;

inline constexpr double kP = 1.0;

inline constexpr wpi::units::volt_t kS = 0.05_V;
inline constexpr kv_unit_t kV = 12_V / kShooterFreeSpeed;
inline constexpr ka_unit_t kA = 0_V * 1_s * 1_s / 1_tr;

inline constexpr double kFeederSpeed = 0.5;
}  // namespace shooter

namespace intake {
inline constexpr int kMotorPort = 6;
inline constexpr std::array<int, 2> kSolenoidPorts = {2, 3};
}  // namespace intake

namespace storage {
inline constexpr int kMotorPort = 7;
inline constexpr int kBallSensorPort = 6;
}  // namespace storage

namespace autonomous {
inline constexpr wpi::units::second_t kTimeout = 3_s;
inline constexpr wpi::units::meter_t kDriveDistance = 2_m;
inline constexpr double kDriveSpeed = 0.5;
}  // namespace autonomous

namespace oi {
inline constexpr int kDriverControllerPort = 0;
}  // namespace oi
}  // namespace constants
