// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/MechanismRoot2d.h"

#include <map>


#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"
#include "frc/util/Color8Bit.h"

using namespace frc;

MechanismRoot2d::MechanismRoot2d(const wpi::Twine& name, double x, double y, const private_init& tag)
    : MechanismObject2d(name.str()), m_x{x}, m_y{y} {}

MechanismRoot2d::MechanismRoot2d(const frc::MechanismRoot2d& rhs) : MechanismObject2d(rhs.GetName()) {}

void MechanismRoot2d::SetPosition(double x, double y) {
  m_x = x;
  m_y = y;
  Flush();
}

void MechanismRoot2d::UpdateEntries(std::shared_ptr<NetworkTable> table) {
  m_posEntry = table->GetEntry(kPosition);
  Flush();
}

inline void MechanismRoot2d::Flush() {
  if (m_posEntry) {
    m_posEntry.SetDoubleArray({m_x, m_y});
  }
}
