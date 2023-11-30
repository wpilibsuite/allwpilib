// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <units/acceleration.h>
#include <units/angle.h>
#include <units/length.h>
#include <units/mass.h>
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

namespace Constants {

static constexpr int kMotorPort = 0;
static constexpr int kEncoderAChannel = 0;
static constexpr int kEncoderBChannel = 1;
static constexpr int kJoystickPort = 0;

static constexpr double kElevatorKp = 0.75;
static constexpr double kElevatorKi = 0.0;
static constexpr double kElevatorKd = 0.0;

static constexpr units::volt_t kElevatorMaxV = 10_V;
static constexpr units::volt_t kElevatorkS = 0.0_V;
static constexpr units::volt_t kElevatorkG = 0.62_V;
static constexpr auto kElevatorkV = 3.9_V / 1_mps;
static constexpr auto kElevatorkA = 0.06_V / 1_mps_sq;

static constexpr double kElevatorGearing = 5.0;
static constexpr units::meter_t kElevatorDrumRadius = 1_in;
static constexpr units::kilogram_t kCarriageMass = 12_lb;

static constexpr units::meter_t kSetpoint = 42.875_in;
static constexpr units::meter_t kLowerSetpoint = 15_in;
static constexpr units::meter_t kMinElevatorHeight = 0_cm;
static constexpr units::meter_t kMaxElevatorHeight = 50_in;

// distance per pulse = (distance per revolution) / (pulses per revolution)
//  = (Pi * D) / ppr
static constexpr double kArmEncoderDistPerPulse =
    2.0 * std::numbers::pi * kElevatorDrumRadius.value() / 4096.0;

}  // namespace Constants
