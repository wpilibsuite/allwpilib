/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/SmallVector.h>

#include "CommandBase.h"
#include "CommandHelper.h"
#include "SetUtilities.h"

namespace frc2 {
/**
 * Schedules the given commands when this command is initialized.  Useful for
 * forking off from CommandGroups.  Note that if run from a CommandGroup, the
 * group will not know about the status of the scheduled commands, and will
 * treat this command as finishing instantly.
 */
class ScheduleCommand : public CommandHelper<CommandBase, ScheduleCommand> {
 public:
  /**
   * Creates a new ScheduleCommand that schedules the given commands when
   * initialized.
   *
   * @param toSchedule the commands to schedule
   */
  explicit ScheduleCommand(wpi::ArrayRef<Command*> toSchedule);

  ScheduleCommand(ScheduleCommand&& other) = default;

  ScheduleCommand(const ScheduleCommand& other) = default;

  void Initialize() override;

  bool IsFinished() override;

  bool RunsWhenDisabled() const override;

 private:
  wpi::SmallVector<Command*, 4> m_toSchedule;
};
}  // namespace frc2
