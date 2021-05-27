// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/buttons/NetworkButton.h"

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>

using namespace frc;

NetworkButton::NetworkButton(std::string_view tableName, std::string_view field)
    : NetworkButton(nt::NetworkTableInstance::GetDefault().GetTable(tableName),
                    field) {}

NetworkButton::NetworkButton(std::shared_ptr<nt::NetworkTable> table,
                             std::string_view field)
    : m_entry(table->GetEntry(field)) {}

bool NetworkButton::Get() {
  return m_entry.GetInstance().IsConnected() && m_entry.GetBoolean(false);
}
