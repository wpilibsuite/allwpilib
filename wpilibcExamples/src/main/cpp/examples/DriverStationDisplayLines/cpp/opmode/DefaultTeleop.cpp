// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "opmode/DefaultTeleop.hpp"

#include <iomanip>
#include <sstream>
#include <string>

#include "wpi/driverstation/DriverStationDisplay.hpp"

void DefaultTeleop::Start() {
  wpi::DriverStationDisplay::SetMode(wpi::DriverStationDisplay::Mode::Line);
  m_timer.Restart();
  m_loopCount = 0;
}

void DefaultTeleop::Periodic() {
  std::ostringstream runtime;
  runtime << std::fixed << std::setprecision(1) << m_timer.Get().value()
          << " seconds";

  wpi::DriverStationDisplay::AddLine("DriverStationDisplay line mode");
  wpi::DriverStationDisplay::AddData("Runtime", runtime.str());
  wpi::DriverStationDisplay::AddData("Loop Count",
                                     std::to_string(m_loopCount++));
  wpi::DriverStationDisplay::UpdateLines();
}
