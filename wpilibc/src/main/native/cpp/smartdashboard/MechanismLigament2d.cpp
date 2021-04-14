// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/smartdashboard/MechanismLigament2d.h"
#include <wpi/raw_ostream.h>

using namespace frc;

MechanismLigament2d::MechanismLigament2d(const wpi::Twine& name, const frc::Color8Bit& color, double length, units::degree_t angle, double lineWeight) : MechanismObject2d(name), m_length {length}, m_angle {angle.to<double>()}, m_weight {lineWeight} {
    SetColor(color);
}

void MechanismLigament2d::UpdateEntries(std::shared_ptr<NetworkTable> table) {
  table->GetEntry(".type").SetString("line");
  
  m_colorEntry = std::make_unique<nt::NetworkTableEntry>(table->GetEntry("color"));
  m_angleEntry = std::make_unique<nt::NetworkTableEntry>(table->GetEntry("angle"));
  m_weightEntry = std::make_unique<nt::NetworkTableEntry>(table->GetEntry("weight"));
  m_lengthEntry = std::make_unique<nt::NetworkTableEntry>(table->GetEntry("length"));
  Flush();
}

void MechanismLigament2d::SetColor(const Color8Bit& color) {
  wpi::raw_string_ostream os{m_color};
  os << "#" << std::hex << (color.red << 16 | color.green << 8 | color.blue);
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

double MechanismLigament2d::GetAngle() {
    return m_angle;
}

double MechanismLigament2d::GetLength() {
    return m_length;
}

#define SAFE_WRITE(data, Type) if (m_##data##Entry) {\
m_##data##Entry->Set##Type(m_##data);\
}
void MechanismLigament2d::Flush() {
    SAFE_WRITE(color, String)
    SAFE_WRITE(angle, Double)
    SAFE_WRITE(length, Double)
    SAFE_WRITE(weight, Double)
}
