// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "opmode/ScoreAuto.hpp"

#include <string>

#include "wpi/driverstation/DriverStationDisplay.hpp"

ScoreAuto::ScoreAuto() {
  wpi::DriverStationDisplay::SetMode(wpi::DriverStationDisplay::Mode::Line);
  m_chooser.AddOptions("Target", {"High", "Mid", "Low"});
  m_chooser.AddIntegerOptions("Delay", 0, 5, 1);
  m_chooser.AddDoubleOptions("Speed", 0.25, 1.0, 0.25);
}

void ScoreAuto::DisabledPeriodic() {
  m_chooser.Update();
  wpi::DriverStationDisplay::UpdateLines();
}

void ScoreAuto::Start() {
  m_target = m_chooser.GetSelected("Target");
  m_delay = m_chooser.GetSelectedInteger("Delay");
  m_speed = m_chooser.GetSelectedDouble("Speed");
}

void ScoreAuto::Periodic() {
  wpi::DriverStationDisplay::AddData("Selected Auto", "Score");
  wpi::DriverStationDisplay::AddData("Target", m_target);
  wpi::DriverStationDisplay::AddData("Delay", std::to_string(m_delay));
  wpi::DriverStationDisplay::AddData("Speed", std::to_string(m_speed));
  wpi::DriverStationDisplay::UpdateLines();
}
