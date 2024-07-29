// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

#include <frc/shuffleboard/Shuffleboard.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <units/length.h>

Robot::Robot() {
  // Add the ultrasonic on the "Sensors" tab of the dashboard
  // Data will update automatically
  frc::Shuffleboard::GetTab("Sensors").Add(m_rangeFinder);
}

void Robot::TeleopPeriodic() {
  // We can read the distance
  units::meter_t distance = m_rangeFinder.GetRange();
  // units auto-convert
  units::millimeter_t distanceMillimeters = distance;
  units::inch_t distanceInches = distance;

  // We can also publish the data itself periodically
  frc::SmartDashboard::PutNumber("Distance[mm]", distanceMillimeters.value());
  frc::SmartDashboard::PutNumber("Distance[inch]", distanceInches.value());
}

void Robot::TestInit() {
  // By default, the Ultrasonic class polls all ultrasonic sensors in a
  // round-robin to prevent them from interfering from one another. However,
  // manual polling is also possible -- note that this disables automatic mode!
  m_rangeFinder.Ping();
}

void Robot::TestPeriodic() {
  if (m_rangeFinder.IsRangeValid()) {
    // Data is valid, publish it
    units::millimeter_t distanceMillimeters = m_rangeFinder.GetRange();
    units::inch_t distanceInches = m_rangeFinder.GetRange();
    frc::SmartDashboard::PutNumber("Distance[mm]", distanceMillimeters.value());
    frc::SmartDashboard::PutNumber("Distance[inch]", distanceInches.value());

    // Ping for next measurement
    m_rangeFinder.Ping();
  }
}

void Robot::TestExit() {
  // Enable automatic mode
  frc::Ultrasonic::SetAutomaticMode(true);
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
