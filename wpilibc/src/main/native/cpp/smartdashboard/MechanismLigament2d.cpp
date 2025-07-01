// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/MechanismLigament2d.hpp"

#include <cstdio>

#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/json.hpp"

using namespace wpi;

MechanismLigament2d::MechanismLigament2d(std::string_view name, double length,
                                         wpi::units::degree_t angle,
                                         double lineWeight,
                                         const wpi::util::Color8Bit& color)
    : MechanismObject2d{name},
      m_length{length},
      m_angle{angle.value()},
      m_weight{lineWeight} {
  SetColor(color);
}

void MechanismLigament2d::UpdateTelemetry(wpi::TelemetryTable& table) const {
  table.Log("color", m_color);
  table.Log("angle", m_angle);
  table.Log("weight", m_weight);
  table.Log("length", m_length);

  MechanismObject2d::UpdateTelemetry(table);
}

std::string_view MechanismLigament2d::GetTelemetryType() const {
  return "line";
}

void MechanismLigament2d::SetColor(const wpi::util::Color8Bit& color) {
  std::scoped_lock lock(m_mutex);
  wpi::util::format_to_n_c_str(m_color, sizeof(m_color), "#{:02X}{:02X}{:02X}",
                               color.red, color.green, color.blue);
}

void MechanismLigament2d::SetAngle(wpi::units::degree_t angle) {
  std::scoped_lock lock(m_mutex);
  m_angle = angle.value();
}

void MechanismLigament2d::SetLineWeight(double lineWidth) {
  std::scoped_lock lock(m_mutex);
  m_weight = lineWidth;
}

wpi::util::Color8Bit MechanismLigament2d::GetColor() {
  std::scoped_lock lock(m_mutex);
  unsigned int r = 0, g = 0, b = 0;
  std::sscanf(m_color, "#%02X%02X%02X", &r, &g, &b);
  return {static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)};
}

double MechanismLigament2d::GetAngle() {
  std::scoped_lock lock(m_mutex);
  return m_angle;
}

double MechanismLigament2d::GetLength() {
  std::scoped_lock lock(m_mutex);
  return m_length;
}

double MechanismLigament2d::GetLineWeight() {
  std::scoped_lock lock(m_mutex);
  return m_weight;
}

void MechanismLigament2d::SetLength(double length) {
  std::scoped_lock lock(m_mutex);
  m_length = length;
}
