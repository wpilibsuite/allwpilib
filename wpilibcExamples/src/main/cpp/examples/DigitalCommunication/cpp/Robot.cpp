// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "wpi/driverstation/internal/DriverStationBackend.hpp"

void Robot::RobotPeriodic() {
  // pull alliance port high if on red alliance, pull low if on blue alliance
  m_allianceOutput.Set(wpi::DriverStationBackend::GetAlliance() ==
                       wpi::DriverStationBackend::kRed);

  // pull enabled port high if enabled, low if disabled
  m_enabledOutput.Set(wpi::DriverStationBackend::IsEnabled());

  // pull auto port high if in autonomous, low if in teleop (or disabled)
  m_autonomousOutput.Set(wpi::DriverStationBackend::IsAutonomous());

  // pull alert port high if match time remaining is between 30 and 25 seconds
  auto matchTime = wpi::DriverStationBackend::GetMatchTime();
  m_alertOutput.Set(matchTime <= 30_s && matchTime >= 25_s);
}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
