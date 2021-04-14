// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/smartdashboard/MechanismRoot2d.h"

#include <map>

#include <networktables/NetworkTableEntry.h>

#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"
#include "frc/util/Color8Bit.h"

using namespace frc;

MechanismRoot2d::MechanismRoot2d(const wpi::Twine& name, double x, double y)
    : MechanismObject2d(name.str()), m_pos{x, y} {}

MechanismRoot2d::MechanismRoot2d(const frc::MechanismRoot2d& rhs) : MechanismObject2d(rhs.GetName()) {}

void MechanismRoot2d::SetPosition(double x, double y) {
  m_pos[0] = x;
  m_pos[1] = y;
  Flush();
}

void MechanismRoot2d::UpdateEntries(std::shared_ptr<NetworkTable> table) {
  m_posEntry = std::make_unique<nt::NetworkTableEntry>(table->GetEntry(kPosition));
  Flush();
}

inline void MechanismRoot2d::Flush() const {
  if (m_posEntry) {
    m_posEntry->SetDoubleArray(m_pos);
  }
}