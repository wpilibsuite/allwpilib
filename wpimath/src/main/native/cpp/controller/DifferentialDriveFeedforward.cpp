// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <Eigen/Core>
#include <wpi/math/controller/DifferentialDriveFeedforward.h>
#include <wpi/math/controller/LinearPlantInversionFeedforward.h>

using namespace wpi::math;

DifferentialDriveWheelVoltages DifferentialDriveFeedforward::Calculate(
    units::meters_per_second_t currentLeftVelocity,
    units::meters_per_second_t nextLeftVelocity,
    units::meters_per_second_t currentRightVelocity,
    units::meters_per_second_t nextRightVelocity, units::second_t dt) {
  wpi::math::LinearPlantInversionFeedforward<2, 2> feedforward{m_plant, dt};

  Eigen::Vector2d r{currentLeftVelocity, currentRightVelocity};
  Eigen::Vector2d nextR{nextLeftVelocity, nextRightVelocity};
  auto u = feedforward.Calculate(r, nextR);
  return {units::volt_t{u(0)}, units::volt_t{u(1)}};
}
