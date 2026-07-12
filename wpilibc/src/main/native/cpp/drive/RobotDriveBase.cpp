// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/RobotDriveBase.hpp"

#include <cmath>
#include <cstddef>

using namespace wpi;

RobotDriveBase::RobotDriveBase() {
  SetSafetyEnabled(true);
}

void RobotDriveBase::SetDeadband(double deadband) {
  m_deadband = deadband;
}

void RobotDriveBase::SetMaxOutput(double maxOutput) {
  m_maxOutput = maxOutput;
}

void RobotDriveBase::FeedWatchdog() {
  Feed();
}

void RobotDriveBase::Desaturate(std::span<double> wheelVelocities) {
  double maxMagnitude = std::abs(wheelVelocities[0]);
  for (size_t i = 1; i < wheelVelocities.size(); i++) {
    double temp = std::abs(wheelVelocities[i]);
    if (maxMagnitude < temp) {
      maxMagnitude = temp;
    }
  }
  if (maxMagnitude > 1.0) {
    for (size_t i = 0; i < wheelVelocities.size(); i++) {
      wheelVelocities[i] = wheelVelocities[i] / maxMagnitude;
    }
  }
}
