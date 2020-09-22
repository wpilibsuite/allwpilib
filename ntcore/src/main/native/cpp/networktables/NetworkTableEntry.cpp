/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "networktables/NetworkTableEntry.h"

#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableInstance.h"

using namespace nt;

NetworkTableEntry::NetworkTableEntry(wpi::StringRef tableName,
                                     wpi::StringRef entryName)
    : NetworkTableEntry(NetworkTableInstance::GetDefault(), tableName,
                        entryName) {}

NetworkTableEntry::NetworkTableEntry(NetworkTableInstance inst,
                                     wpi::StringRef tableName,
                                     wpi::StringRef entryName)
    : NetworkTableEntry(inst.GetTable(tableName), entryName) {}

NetworkTableEntry::NetworkTableEntry(std::shared_ptr<NetworkTable> table,
                                     wpi::StringRef entryName)
    : m_handle{table->GetEntry(entryName).GetHandle()} {}

NetworkTableInstance NetworkTableEntry::GetInstance() const {
  return NetworkTableInstance{GetInstanceFromHandle(m_handle)};
}
