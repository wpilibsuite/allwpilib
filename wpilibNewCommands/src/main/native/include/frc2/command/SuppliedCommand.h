// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>
#include <memory>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * Lazily initializes the command supplied. This can be useful for initializing
 * the command depending on a value known at compile-time without constructing a
 * map for {@link SelectCommand}.
 */
class SuppliedCommand : public CommandHelper<CommandBase, SuppliedCommand> {
 public:
  /**
   * Creates a new SuppliedCommand.
   *
   * @param supplier a supplier providing the command to run
   * @param requirements the subsystems the selected command might require
   */
  explicit SuppliedCommand(std::function<Command*()> supplier,
                           std::initializer_list<Subsystem*> requirements);

  SuppliedCommand(SuppliedCommand&& other) = default;

  SuppliedCommand(const SuppliedCommand& other) = default;

  void Initialize() final;
  void Execute() final;
  void End(bool interrupted) final;
  bool IsFinished() final;

 protected:
  std::unique_ptr<Command> TransferOwnership() && override;

 private:
  std::function<Command*()> m_supplier;
  Command* m_selectedCommand;
};
}  // namespace frc2
