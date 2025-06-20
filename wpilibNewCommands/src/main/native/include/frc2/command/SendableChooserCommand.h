// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/FunctionExtras.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/Requirements.h"

namespace frc2 {

class SendableChooserCommand
    : public CommandHelper<Command, SendableChooserCommand> {
 public:
  SendableChooserCommand(wpi::unique_function<CommandPtr()> supplier,
                         Requirements requirements);

  SendableChooserCommand(SendableChooserCommand&& other) = default;

  void Initialize() override;

 private:
  wpi::unique_function<CommandPtr()> m_supplier;
};
}  // namespace frc2
