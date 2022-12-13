// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/DifferentialDriveFeedforward.h"

#include "frc/EigenCore.h"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/system/plant/LinearSystemId.h"

using namespace frc;

DifferentialDriveFeedforward::DifferentialDriveFeedforward(
    decltype(1_V / 1_mps) kVLinear, decltype(1_V / 1_mps_sq) kALinear,
    decltype(1_V / 1_rad_per_s) kVAngular,
    decltype(1_V / 1_rad_per_s_sq) kAAngular, units::meter_t trackwidth)
    : m_plant{frc::LinearSystemId::IdentifyDrivetrainSystem(
          kVLinear, kALinear, kVAngular, kAAngular, trackwidth)} {}

DifferentialDriveFeedforward::DifferentialDriveFeedforward(
    decltype(1_V / 1_mps) kVLinear, decltype(1_V / 1_mps_sq) kALinear,
    decltype(1_V / 1_mps) kVAngular, decltype(1_V / 1_mps_sq) kAAngular)
    : m_plant{frc::LinearSystemId::IdentifyDrivetrainSystem(
          kVLinear, kALinear, kVAngular, kAAngular)} {}

DifferentialDriveWheelVoltages DifferentialDriveFeedforward::Calculate(
    units::meters_per_second_t currentLeftVelocity,
    units::meters_per_second_t nextLeftVelocity,
    units::meters_per_second_t currentRightVelocity,
    units::meters_per_second_t nextRightVelocity, units::second_t dt) {
  frc::LinearPlantInversionFeedforward<2, 2> feedforward{m_plant, dt};

  frc::Vectord<2> r{currentLeftVelocity, currentRightVelocity};
  frc::Vectord<2> nextR{nextLeftVelocity, nextRightVelocity};
  auto u = feedforward.Calculate(r, nextR);
  return {units::volt_t{u(0)}, units::volt_t{u(1)}};
}
