/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/button/NetworkButton.h"

using namespace frc2;

NetworkButton::NetworkButton(nt::NetworkTableEntry entry)
    : Button([entry] {
        return entry.GetInstance().IsConnected() && entry.GetBoolean(false);
      }) {}

NetworkButton::NetworkButton(std::shared_ptr<nt::NetworkTable> table,
                             const wpi::Twine& field)
    : NetworkButton(table->GetEntry(field)) {}

NetworkButton::NetworkButton(const wpi::Twine& table, const wpi::Twine& field)
    : NetworkButton(nt::NetworkTableInstance::GetDefault().GetTable(table),
                    field) {}
