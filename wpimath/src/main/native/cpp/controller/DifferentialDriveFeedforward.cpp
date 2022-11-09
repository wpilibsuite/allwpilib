// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/DifferentialDriveFeedforward.h"

frc::DifferentialDriveWheelVoltages
frc::DifferentialDriveFeedforward::Calculate(
    units::meters_per_second_t currentLVelocity,
    units::meters_per_second_t nextLVelocity,
    units::meters_per_second_t currentRVelocity,
    units::meters_per_second_t nextRVelocity, units::second_t dt) {
  frc::LinearPlantInversionFeedforward<2, 2> feedforward{m_plant, dt};

  frc::Vectord<2> r{currentLVelocity, currentRVelocity};
  frc::Vectord<2> nextR{nextLVelocity, nextRVelocity};
  auto u = feedforward.Calculate(r, nextR);
  return {units::volt_t{u(0)}, units::volt_t{u(1)}};
}
