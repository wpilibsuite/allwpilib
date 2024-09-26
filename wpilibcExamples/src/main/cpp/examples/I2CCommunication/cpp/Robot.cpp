// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

#include <string>

#include <fmt/format.h>
#include <frc/DriverStation.h>
#include <frc/Timer.h>

void Robot::RobotPeriodic() {
  // Creates a string to hold current robot state information, including
  // alliance, enabled state, operation mode, and match time. The message
  // is sent in format "AEM###" where A is the alliance color, (R)ed or
  // (B)lue, E is the enabled state, (E)nabled or (D)isabled, M is the
  // operation mode, (A)utonomous or (T)eleop, and ### is the zero-padded
  // time remaining in the match.
  //
  // For example, "RET043" would indicate that the robot is on the red
  // alliance, enabled in teleop mode, with 43 seconds left in the match.

  std::string allianceString = "U";
  auto alliance = frc::DriverStation::GetAlliance();
  if (alliance.has_value()) {
    allianceString = alliance == frc::DriverStation::Alliance::kRed ? "R" : "B";
  }

  auto string =
      fmt::format("{}{}{}{:03}", allianceString,
                  frc::DriverStation::IsEnabled() ? "E" : "D",
                  frc::DriverStation::IsAutonomous() ? "A" : "T",
                  static_cast<int>(frc::Timer::GetMatchTime().value()));

  arduino.WriteBulk(reinterpret_cast<uint8_t*>(string.data()), string.size());
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
