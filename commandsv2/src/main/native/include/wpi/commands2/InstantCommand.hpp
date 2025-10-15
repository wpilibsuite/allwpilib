// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "wpi/commands2/CommandHelper.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"
#include "wpi/commands2/Requirements.hpp"

namespace frc2 {
/**
 * A Command that runs instantly; it will initialize, execute once, and end on
 * the same iteration of the scheduler.  Users can either pass in a Runnable and
 * a set of requirements, or else subclass this command if desired.
 *
 * This class is provided by the NewCommands VendorDep
 */
class InstantCommand : public CommandHelper<FunctionalCommand, InstantCommand> {
 public:
  /**
   * Creates a new InstantCommand that runs the given Runnable with the given
   * requirements.
   *
   * @param toRun        the Runnable to run
   * @param requirements the subsystems required by this command
   */
  explicit InstantCommand(std::function<void()> toRun,
                          Requirements requirements = {});

  InstantCommand(InstantCommand&& other) = default;

  InstantCommand(const InstantCommand& other) = default;

  /**
   * Creates a new InstantCommand with a Runnable that does nothing.  Useful
   * only as a no-arg constructor to call implicitly from subclass constructors.
   */
  InstantCommand();
};
}  // namespace frc2
