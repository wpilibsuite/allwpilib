// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/MechanismObject2d.h"

#include <string>

using namespace frc;

MechanismObject2d::MechanismObject2d(std::string_view name) : m_name{name} {}

const std::string& MechanismObject2d::GetName() const {
  return m_name;
}

void MechanismObject2d::Update(std::shared_ptr<nt::NetworkTable> table) {
  std::scoped_lock lock(m_mutex);
  m_table = table;
  UpdateEntries(m_table);
  for (const auto& entry : m_objects) {
    entry.second->Update(m_table->GetSubTable(entry.first));
  }
}
