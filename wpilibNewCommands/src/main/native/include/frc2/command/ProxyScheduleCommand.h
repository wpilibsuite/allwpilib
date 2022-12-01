// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <span>

#include <wpi/SmallVector.h>
#include <wpi/deprecated.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/SetUtilities.h"

namespace frc2 {
/**
 * Schedules the given commands when this command is initialized, and ends when
 * all the commands are no longer scheduled.  Useful for forking off from
 * CommandGroups.  If this command is interrupted, it will cancel all of the
 * commands.
 *
 * This class is provided by the NewCommands VendorDep
 */
class ProxyScheduleCommand
    : public CommandHelper<CommandBase, ProxyScheduleCommand> {
 public:
  /**
   * Creates a new ProxyScheduleCommand that schedules the given commands when
   * initialized, and ends when they are all no longer scheduled.
   *
   * @param toSchedule the commands to schedule
   * @deprecated Replace with {@link ProxyCommand},
   * composing multiple of them in a {@link ParallelRaceGroup} if needed.
   */
  WPI_DEPRECATED("Replace with ProxyCommand")
  explicit ProxyScheduleCommand(std::span<Command* const> toSchedule);

  explicit ProxyScheduleCommand(Command* toSchedule);

  ProxyScheduleCommand(ProxyScheduleCommand&& other) = default;

  void Initialize() override;

  void End(bool interrupted) override;

  void Execute() override;

  bool IsFinished() override;

 private:
  wpi::SmallVector<Command*, 4> m_toSchedule;
  std::unique_ptr<Command> m_owning;
  bool m_finished{false};
};
}  // namespace frc2
