// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/FunctionExtras.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc2/command/CommandHelper.h"

namespace frc2 {

class DynamicCommand : public CommandHelper<Command, DynamicCommand> {
 public:
  explicit DynamicCommand(wpi::unique_function<Command*()> supplier);
  explicit DynamicCommand(wpi::unique_function<CommandPtr()> supplier);

  void Initialize() override;
  void End(bool interrupted) override;
  bool IsFinished() override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  wpi::unique_function<Command*()> m_supplier;
  Command* m_command;
};

}  // namespace frc2
