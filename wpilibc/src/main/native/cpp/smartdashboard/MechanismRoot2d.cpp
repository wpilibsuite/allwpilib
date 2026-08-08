// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/MechanismRoot2d.hpp"

#include "wpi/telemetry/TelemetryTable.hpp"

using namespace wpi;

MechanismRoot2d::MechanismRoot2d(std::string_view name, double x, double y,
                                 const private_init&)
    : MechanismObject2d{name}, m_x{x}, m_y{y} {}

void MechanismRoot2d::SetPosition(double x, double y) {
  std::scoped_lock lock(m_mutex);
  m_x = x;
  m_y = y;
}

void MechanismRoot2d::LogTo(wpi::TelemetryTable& table) const {
  table.Log("position", {m_x, m_y});
  MechanismObject2d::LogTo(table);
}
