// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>

#include <wpi/SmallVector.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/SetUtilities.h"

namespace frc2 {
/**
 * Schedules the given commands when this command is initialized.  Useful for
 * forking off from CommandGroups.  Note that if run from a CommandGroup, the
 * group will not know about the status of the scheduled commands, and will
 * treat this command as finishing instantly.
 *
 * This class is provided by the NewCommands VendorDep
 */
class ScheduleCommand : public CommandHelper<CommandBase, ScheduleCommand> {
 public:
  /**
   * Creates a new ScheduleCommand that schedules the given commands when
   * initialized.
   *
   * @param toSchedule the commands to schedule
   */
  explicit ScheduleCommand(std::span<Command* const> toSchedule);

  explicit ScheduleCommand(Command* toSchedule);

  ScheduleCommand(ScheduleCommand&& other) = default;

  ScheduleCommand(const ScheduleCommand& other) = default;

  void Initialize() override;

  bool IsFinished() override;

  bool RunsWhenDisabled() const override;

 private:
  wpi::SmallVector<Command*, 4> m_toSchedule;
};
}  // namespace frc2
