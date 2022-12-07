// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <memory>
#include <span>
#include <vector>

#include "frc2/command/CommandBase.h"

namespace frc2 {

/**
 * A base for CommandGroups.  Statically tracks commands that have been
 * allocated to groups to ensure those commands are not also used independently,
 * which can result in inconsistent command state and unpredictable execution.
 *
 * This class is provided by the NewCommands VendorDep
 */
class CommandGroupBase : public CommandBase {
 public:
  /**
   * Requires that the specified command not have been already allocated to a
   * CommandGroup. Reports an error if the command is already grouped.
   *
   * @param command The command to check
   * @return True if all the command is ungrouped.
   */
  static bool RequireUngrouped(const Command& command);

  /**
   * Requires that the specified command not have been already allocated to a
   * CommandGroup. Reports an error if the command is already grouped.
   *
   * @param command The command to check
   * @return True if all the command is ungrouped.
   */
  static bool RequireUngrouped(const Command* command);

  /**
   * Requires that the specified commands not have been already allocated to a
   * CommandGroup. Reports an error if any of the commands are already grouped.
   *
   * @param commands The commands to check
   * @return True if all the commands are ungrouped.
   */
  static bool RequireUngrouped(
      std::span<const std::unique_ptr<Command>> commands);

  /**
   * Requires that the specified commands not have been already allocated to a
   * CommandGroup. Reports an error if any of the commands are already grouped.
   *
   * @param commands The commands to check
   * @return True if all the commands are ungrouped.
   */
  static bool RequireUngrouped(std::initializer_list<const Command*> commands);

  /**
   * Adds the given commands to the command group.
   *
   * @param commands The commands to add.
   */
  virtual void AddCommands(
      std::vector<std::unique_ptr<Command>>&& commands) = 0;
};
}  // namespace frc2
