// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/MechanismObject2d.h"

using namespace frc;

MechanismObject2d::MechanismObject2d(wpi::Twine& name) : m_name{name} {}

wpi::Twine& MechanismObject2d::GetName() const {
  return m_name;
}

bool MechanismObject2d::operator==(MechanismObject2d& rhs) {
  return m_name == rhs.m_name;
}

void MechanismObject2d::Update(std::shared_ptr<NetworkTable> table) {
  m_table = table;
  UpdateEntries(m_table);
  for (auto& [name, object] : m_objects) {
    object.Update(m_table->GetSubTable(name));
  }
}
