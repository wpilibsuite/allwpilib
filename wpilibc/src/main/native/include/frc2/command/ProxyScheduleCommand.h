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
 * Schedules the given commands when this command is initialized, and ends when
 * all the commands are no longer scheduled.  Useful for forking off from
 * CommandGroups.  If this command is interrupted, it will cancel all of the
 * commands.
 */
class ProxyScheduleCommand
    : public CommandHelper<CommandBase, ProxyScheduleCommand> {
 public:
  /**
   * Creates a new ProxyScheduleCommand that schedules the given commands when
   * initialized, and ends when they are all no longer scheduled.
   *
   * @param toSchedule the commands to schedule
   */
  explicit ProxyScheduleCommand(wpi::ArrayRef<Command*> toSchedule);

  ProxyScheduleCommand(ProxyScheduleCommand&& other) = default;

  ProxyScheduleCommand(const ProxyScheduleCommand& other) = default;

  void Initialize() override;

  void End(bool interrupted) override;

  void Execute() override;

  bool IsFinished() override;

 private:
  wpi::SmallVector<Command*, 4> m_toSchedule;
  bool m_finished{false};
};
}  // namespace frc2
