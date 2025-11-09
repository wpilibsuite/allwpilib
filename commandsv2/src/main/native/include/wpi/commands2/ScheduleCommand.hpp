// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>

#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandHelper.hpp"
#include "wpi/util/SmallSet.hpp"

namespace wpi::cmd {
/**
 * Schedules the given commands when this command is initialized. Useful for
 * forking off from CommandGroups. Note that if run from a composition, the
 * composition will not know about the status of the scheduled commands, and
 * will treat this command as finishing instantly.
 *
 * This class is provided by the NewCommands VendorDep
 */
class ScheduleCommand : public CommandHelper<Command, ScheduleCommand> {
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
  wpi::util::SmallSet<Command*, 4> m_toSchedule;
};
}  // namespace wpi::cmd
