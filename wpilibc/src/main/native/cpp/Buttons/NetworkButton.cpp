/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Buttons/NetworkButton.h"

#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableInstance.h"

using namespace frc;

NetworkButton::NetworkButton(llvm::StringRef tableName, llvm::StringRef field)
    : NetworkButton(nt::NetworkTableInstance::GetDefault().GetTable(tableName),
                    field) {}

NetworkButton::NetworkButton(std::shared_ptr<nt::NetworkTable> table,
                             llvm::StringRef field)
    : m_entry(table->GetEntry(field)) {}

bool NetworkButton::Get() {
  return m_entry.GetInstance().IsConnected() && m_entry.GetBoolean(false);
}
