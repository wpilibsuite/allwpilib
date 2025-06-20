// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/SendableChooserCommand.h"

#include <utility>

using namespace frc2;

SendableChooserCommand::SendableChooserCommand(
    wpi::unique_function<CommandPtr()> supplier, Requirements requirements)
    : m_supplier{std::move(supplier)} {
  AddRequirements(requirements);
}

void SendableChooserCommand::Initialize() {
  CommandScheduler::GetInstance().Schedule(m_supplier().AsProxy());
}
