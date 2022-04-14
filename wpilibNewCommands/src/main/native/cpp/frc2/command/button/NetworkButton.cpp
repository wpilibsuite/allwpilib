// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/NetworkButton.h"

using namespace frc2;

NetworkButton::NetworkButton(nt::NetworkTableEntry entry)
    : Button([entry] {
        return entry.GetInstance().IsConnected() && entry.GetBoolean(false);
      }) {}

NetworkButton::NetworkButton(std::shared_ptr<nt::NetworkTable> table,
                             std::string_view field)
    : NetworkButton(table->GetEntry(field)) {}

NetworkButton::NetworkButton(std::string_view table, std::string_view field)
    : NetworkButton(nt::NetworkTableInstance::GetDefault().GetTable(table),
                    field) {}
