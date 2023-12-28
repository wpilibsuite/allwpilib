// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <numbers>

#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/time.h>
#include <units/voltage.h>

namespace constants {
namespace drive {
static constexpr int kLeftMotor1Port = 0;
static constexpr int kLeftMotor2Port = 1;
static constexpr int kRightMotor1Port = 2;
static constexpr int kRightMotor2Port = 3;

static constexpr std::array<int, 2> kLeftEncoderPorts = {0, 1};
static constexpr std::array<int, 2> kRightEncoderPorts = {2, 3};
static constexpr bool kLeftEncoderReversed = false;
static constexpr bool kRightEncoderReversed = true;

static constexpr int kEncoderCpr = 1024;
static constexpr units::meter_t kWheelDiameter = 6_in;
static constexpr units::meter_t kEncoderDistancePerPulse =
    (kWheelDiameter * std::numbers::pi) / static_cast<double>(kEncoderCpr);
}  // namespace drive

namespace shooter {

using kv_unit =
    units::compound_unit<units::compound_unit<units::volts, units::seconds>,
                         units::inverse<units::turns>>;
using kv_unit_t = units::unit_t<kv_unit>;

static constexpr std::array<int, 2> kEncoderPorts = {4, 5};
static constexpr bool kLeftEncoderReversed = false;
static constexpr int kEncoderCpr = 1024;
static constexpr units::turn_t kEncoderDistancePerPulse =
    units::turn_t{1.0} / static_cast<double>(kEncoderCpr);

static constexpr int kShooterMotorPort = 4;
static constexpr int kFeederMotorPort = 5;

static constexpr units::turns_per_second_t kShooterFreeSpeed = 5300_tps;
static constexpr units::turns_per_second_t kShooterTargetSpeed = 4000_tps;
static constexpr units::turns_per_second_t kShooterTolerance = 50_tps;

static constexpr double kP = 1.0;

static constexpr units::volt_t kS = 0.05_V;
static constexpr kv_unit_t kV = (12_V) / kShooterFreeSpeed;

static constexpr double kFeederSpeed = 0.5;
}  // namespace shooter

namespace intake {
static constexpr int kMotorPort = 6;
static constexpr std::array<int, 2> kSolenoidPorts = {2, 3};
}  // namespace intake

namespace storage {
static constexpr int kMotorPort = 7;
static constexpr int kBallSensorPort = 6;
}  // namespace storage

namespace autonomous {
static constexpr units::second_t kTimeout = 3_s;
static constexpr units::meter_t kDriveDistance = 2_m;
static constexpr double kDriveSpeed = 0.5;
}  // namespace autonomous

namespace oi {
static constexpr int kDriverControllerPort = 0;
}  // namespace oi
}  // namespace constants
