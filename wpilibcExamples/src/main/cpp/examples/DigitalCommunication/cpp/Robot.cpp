// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include <wpi/driverstation/DriverStation.hpp>

void Robot::RobotPeriodic() {
  // pull alliance port high if on red alliance, pull low if on blue alliance
  m_allianceOutput.Set(frc::DriverStation::GetAlliance() ==
                       frc::DriverStation::kRed);

  // pull enabled port high if enabled, low if disabled
  m_enabledOutput.Set(frc::DriverStation::IsEnabled());

  // pull auto port high if in autonomous, low if in teleop (or disabled)
  m_autonomousOutput.Set(frc::DriverStation::IsAutonomous());

  // pull alert port high if match time remaining is between 30 and 25 seconds
  auto matchTime = frc::DriverStation::GetMatchTime();
  m_alertOutput.Set(matchTime <= 30_s && matchTime >= 25_s);
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
