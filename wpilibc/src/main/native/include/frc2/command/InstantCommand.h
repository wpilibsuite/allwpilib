/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "CommandBase.h"
#include "CommandHelper.h"

namespace frc2 {
/**
 * A Command that runs instantly; it will initialize, execute once, and end on
 * the same iteration of the scheduler.  Users can either pass in a Runnable and
 * a set of requirements, or else subclass this command if desired.
 */
class InstantCommand : public CommandHelper<CommandBase, InstantCommand> {
 public:
  /**
   * Creates a new InstantCommand that runs the given Runnable with the given
   * requirements.
   *
   * @param toRun        the Runnable to run
   * @param requirements the subsystems required by this command
   */
  InstantCommand(std::function<void()> toRun,
                 std::initializer_list<Subsystem*> requirements);

  InstantCommand(InstantCommand&& other) = default;

  InstantCommand(const InstantCommand& other) = default;

  /**
   * Creates a new InstantCommand with a Runnable that does nothing.  Useful
   * only as a no-arg constructor to call implicitly from subclass constructors.
   */
  InstantCommand();

  void Initialize() override;

  bool IsFinished() final;

 private:
  std::function<void()> m_toRun;
};
}  // namespace frc2
