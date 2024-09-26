// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/MechanismLigament2d.h"

#include <cstdio>
#include <memory>

#include <wpi/StringExtras.h>
#include <wpi/json.h>

using namespace frc;

static constexpr std::string_view kSmartDashboardType = "line";

MechanismLigament2d::MechanismLigament2d(std::string_view name, double length,
                                         units::degree_t angle,
                                         double lineWeight,
                                         const frc::Color8Bit& color)
    : MechanismObject2d{name},
      m_length{length},
      m_angle{angle.value()},
      m_weight{lineWeight} {
  SetColor(color);
}

void MechanismLigament2d::UpdateEntries(
    std::shared_ptr<nt::NetworkTable> table) {
  m_typePub = table->GetStringTopic(".type").PublishEx(
      nt::StringTopic::kTypeString, {{"SmartDashboard", kSmartDashboardType}});
  m_typePub.Set(kSmartDashboardType);

  m_colorEntry = table->GetStringTopic("color").GetEntry("");
  m_colorEntry.Set(m_color);
  m_angleEntry = table->GetDoubleTopic("angle").GetEntry(0.0);
  m_angleEntry.Set(m_angle);
  m_weightEntry = table->GetDoubleTopic("weight").GetEntry(0.0);
  m_weightEntry.Set(m_weight);
  m_lengthEntry = table->GetDoubleTopic("length").GetEntry(0.0);
  m_lengthEntry.Set(m_length);
}

void MechanismLigament2d::SetColor(const Color8Bit& color) {
  std::scoped_lock lock(m_mutex);

  wpi::format_to_n_c_str(m_color, sizeof(m_color), "#{:02X}{:02X}{:02X}",
                         color.red, color.green, color.blue);

  if (m_colorEntry) {
    m_colorEntry.Set(m_color);
  }
}

void MechanismLigament2d::SetAngle(units::degree_t angle) {
  std::scoped_lock lock(m_mutex);
  m_angle = angle.value();
  if (m_angleEntry) {
    m_angleEntry.Set(m_angle);
  }
}

void MechanismLigament2d::SetLineWeight(double lineWidth) {
  std::scoped_lock lock(m_mutex);
  m_weight = lineWidth;
  if (m_weightEntry) {
    m_weightEntry.Set(m_weight);
  }
}

Color8Bit MechanismLigament2d::GetColor() {
  std::scoped_lock lock(m_mutex);
  if (m_colorEntry) {
    auto color = m_colorEntry.Get();
    std::strncpy(m_color, color.c_str(), sizeof(m_color));
    m_color[sizeof(m_color) - 1] = '\0';
  }
  unsigned int r = 0, g = 0, b = 0;
  std::sscanf(m_color, "#%02X%02X%02X", &r, &g, &b);
  return {static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)};
}

double MechanismLigament2d::GetAngle() {
  std::scoped_lock lock(m_mutex);
  if (m_angleEntry) {
    m_angle = m_angleEntry.Get();
  }
  return m_angle;
}

double MechanismLigament2d::GetLength() {
  std::scoped_lock lock(m_mutex);
  if (m_lengthEntry) {
    m_length = m_lengthEntry.Get();
  }
  return m_length;
}

double MechanismLigament2d::GetLineWeight() {
  std::scoped_lock lock(m_mutex);
  if (m_weightEntry) {
    m_weight = m_weightEntry.Get();
  }
  return m_weight;
}

void MechanismLigament2d::SetLength(double length) {
  std::scoped_lock lock(m_mutex);
  m_length = length;
  if (m_lengthEntry) {
    m_lengthEntry.Set(length);
  }
}
