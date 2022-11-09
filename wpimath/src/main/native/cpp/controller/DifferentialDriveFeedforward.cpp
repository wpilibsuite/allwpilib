// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/DifferentialDriveFeedforward.h"

frc::DifferentialDriveWheelVoltages
frc::DifferentialDriveFeedforward::Calculate(double currentLVelocity,
                                             double nextLVelocity,
                                             double currentRVelocity,
                                             double nextRVelocity,
                                             units::second_t dtSeconds) {
  frc::LinearPlantInversionFeedforward<2, 2> feedforward{
      frc::DifferentialDriveFeedforward::m_plant, dtSeconds};

  frc::Vectord<2> r{currentLVelocity, currentRVelocity};
  frc::Vectord<2> nextR{nextLVelocity, nextRVelocity};
  auto u = feedforward.Calculate(r, nextR);
  frc::DifferentialDriveWheelVoltages voltages;
  voltages.left = units::volt_t{u[0]};
  voltages.right = units::volt_t{u[1]};
  return voltages;
}
