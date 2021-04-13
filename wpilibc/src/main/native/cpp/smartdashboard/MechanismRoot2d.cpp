// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "MechanismRoot2d.h"

#include <map>

#include <networktables/NetworkTableEntry.h>

#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"
#include "frc/util/Color8Bit.h"

using namespace frc;

MechanismRoot2d::MechanismRoot2d(wpi::Twine& name, double x, double y)
    : m_name{name}, m_pos{x, y} {}

void MechanismRoot2d::SetPosition(double x, double y) {
  m_pos[0] = x;
  m_pos[1] = y;
}

void MechanismRoot2d::Update(NetworkTable& table) {
  m_table = std::make_shared<NetworkTable>(table);

  if (m_table) {
    m_table->GetEntry(kPosition).SetDoubleArray(m_pos);
  }
}
