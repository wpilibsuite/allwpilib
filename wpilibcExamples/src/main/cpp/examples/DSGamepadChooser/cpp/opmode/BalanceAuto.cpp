// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "opmode/BalanceAuto.hpp"

#include <string>

#include "wpi/driverstation/DriverStationDisplay.hpp"

BalanceAuto::BalanceAuto() {
  wpi::DriverStationDisplay::SetMode(wpi::DriverStationDisplay::Mode::Line);
  m_chooser.AddOptions("Approach", {"Left", "Center", "Right"});
  m_chooser.AddIntegerOptions("Attempts", 1, 3, 1);
  m_chooser.AddDoubleOptions("Turn Scale", 0.2, 1.0, 0.2);
}

void BalanceAuto::DisabledPeriodic() {
  m_chooser.Update();
  wpi::DriverStationDisplay::UpdateLines();
}

void BalanceAuto::Start() {
  m_approach = m_chooser.GetSelected("Approach");
  m_attempts = m_chooser.GetSelectedInteger("Attempts");
  m_turnScale = m_chooser.GetSelectedDouble("Turn Scale");
}

void BalanceAuto::Periodic() {
  wpi::DriverStationDisplay::AddData("Selected Auto", "Balance");
  wpi::DriverStationDisplay::AddData("Approach", m_approach);
  wpi::DriverStationDisplay::AddData("Attempts", std::to_string(m_attempts));
  wpi::DriverStationDisplay::AddData("Turn Scale", std::to_string(m_turnScale));
  wpi::DriverStationDisplay::UpdateLines();
}
