// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/DifferentialDriveFeedforward.h"

#include <Eigen/Core>

#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/system/plant/LinearSystemId.h"

using namespace frc;

DifferentialDriveFeedforward::DifferentialDriveFeedforward(
    decltype(1_V / 1_mps) kVLinear, decltype(1_V / 1_mps_sq) kALinear,
    decltype(1_V / 1_rad_per_s) kVAngular,
    decltype(1_V / 1_rad_per_s_sq) kAAngular, units::meter_t trackwidth)
    // See LinearSystemId::IdentifyDrivetrainSystem(decltype(1_V / 1_mps),
    // decltype(1_V / 1_mps_sq), decltype(1_V / 1_rad_per_s), decltype(1_V /
    // 1_rad_per_s_sq))
    : DifferentialDriveFeedforward{kVLinear, kALinear,
                                   kVAngular * 2.0 / trackwidth * 1_rad,
                                   kAAngular * 2.0 / trackwidth * 1_rad} {}

DifferentialDriveFeedforward::DifferentialDriveFeedforward(
    decltype(1_V / 1_mps) kVLinear, decltype(1_V / 1_mps_sq) kALinear,
    decltype(1_V / 1_mps) kVAngular, decltype(1_V / 1_mps_sq) kAAngular)
    : m_plant{frc::LinearSystemId::IdentifyDrivetrainSystem(
          kVLinear, kALinear, kVAngular, kAAngular)},
      m_kVLinear{kVLinear},
      m_kALinear{kALinear},
      m_kVAngular{kVAngular},
      m_kAAngular{kAAngular} {}

DifferentialDriveWheelVoltages DifferentialDriveFeedforward::Calculate(
    units::meters_per_second_t currentLeftVelocity,
    units::meters_per_second_t nextLeftVelocity,
    units::meters_per_second_t currentRightVelocity,
    units::meters_per_second_t nextRightVelocity, units::second_t dt) {
  frc::LinearPlantInversionFeedforward<2, 2> feedforward{m_plant, dt};

  Eigen::Vector2d r{currentLeftVelocity, currentRightVelocity};
  Eigen::Vector2d nextR{nextLeftVelocity, nextRightVelocity};
  auto u = feedforward.Calculate(r, nextR);
  return {units::volt_t{u(0)}, units::volt_t{u(1)}};
}
