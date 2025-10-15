// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

/**
 * A Command that runs instantly; it will initialize, execute once, and end on the same iteration of
 * the scheduler. Users can either pass in a Runnable and a set of requirements, or else subclass
 * this command if desired.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class InstantCommand extends FunctionalCommand {
  /**
   * Creates a new InstantCommand that runs the given Runnable with the given requirements.
   *
   * @param toRun the Runnable to run
   * @param requirements the subsystems required by this command
   */
  public InstantCommand(Runnable toRun, Subsystem... requirements) {
    super(toRun, () -> {}, interrupted -> {}, () -> true, requirements);
  }

  /**
   * Creates a new InstantCommand with a Runnable that does nothing. Useful only as a no-arg
   * constructor to call implicitly from subclass constructors.
   */
  public InstantCommand() {
    this(() -> {});
  }
}
