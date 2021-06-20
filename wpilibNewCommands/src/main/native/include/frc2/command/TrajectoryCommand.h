// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>
#include <memory>

#include <frc/Timer.h>
#include <frc/trajectory/Trajectory.h>
#include <wpi/span.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command that follows a certain trajectory, passing the target state to an
 * output function every iteration.
 *
 * <p>This command does <b>not</b> handle Ramsete, PID, and Feedforward control
 * internally - they should be used by the subsystem in the output function.
 */
class TrajectoryCommand : public CommandHelper<CommandBase, TrajectoryCommand> {
 public:
  /**
   * Create a new TrajectoryCommand.
   *
   * @param trajectory The trajectory to follow.
   * @param output A function that consumes the target state for each timestep.
   * This state can be fed into a Ramsete controller or similar.
   * @param requirements The subsystems to require.
   */
  TrajectoryCommand(frc::Trajectory trajectory,
                    std::function<void(const frc::Trajectory::State&)> output,
                    std::initializer_list<Subsystem* const> requirements);

  /**
   * Create a new TrajectoryCommand.
   *
   * @param trajectory The trajectory to follow.
   * @param output A function that consumes the target state for each timestep.
   * This state can be fed into a Ramsete controller or similar.
   * @param requirements The subsystems to require.
   */
  TrajectoryCommand(frc::Trajectory trajectory,
                    std::function<void(const frc::Trajectory::State&)> output,
                    wpi::span<Subsystem* const> requirements = {});

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

 private:
  frc::Trajectory m_trajectory;
  std::function<void(const frc::Trajectory::State&)> m_output;
  frc::Timer m_timer;
};
}  // namespace frc2
