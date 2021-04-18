// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/MechanismLigament2d.h"
#include <wpi/raw_ostream.h>

using namespace frc;

MechanismLigament2d::MechanismLigament2d(const wpi::Twine& name, const frc::Color8Bit& color,
                                         double length, units::degree_t angle, double lineWeight)
                                         : MechanismObject2d(name), m_length {length},
                                         m_angle {angle.to<double>()}, m_weight {lineWeight} {
  SetColor(color);
}

void MechanismLigament2d::UpdateEntries(std::shared_ptr<NetworkTable> table) {
  table->GetEntry(".type").SetString("line");

  m_colorEntry = table->GetEntry("color");
  m_angleEntry = table->GetEntry("angle");
  m_weightEntry = table->GetEntry("weight");
  m_lengthEntry = table->GetEntry("length");
  Flush();
}

void MechanismLigament2d::SetColor(const Color8Bit& color) {
  std::snprintf(m_color, sizeof(m_color), "#%02X%02X%02X", color.red, color.green, color.blue);
  Flush();
}

void MechanismLigament2d::SetAngle(units::degree_t angle) {
  m_angle = angle.to<double>();
  Flush();
}

void MechanismLigament2d::SetLineWeight(double lineWidth) {
  m_weight = lineWidth;
  Flush();
}

double MechanismLigament2d::GetAngle() const {
  return m_angle;
}

double MechanismLigament2d::GetLength() const {
  return m_length;
}

void MechanismLigament2d::SetLength(double length) {
  m_length = length;
  Flush();
}

#define SAFE_WRITE(data, Type)           \
  if (m_##data##Entry) {                 \
    m_##data##Entry.Set##Type(m_##data); \
  }
void MechanismLigament2d::Flush() {
  SAFE_WRITE(color, String)
  SAFE_WRITE(angle, Double)
  SAFE_WRITE(length, Double)
  SAFE_WRITE(weight, Double)
}
