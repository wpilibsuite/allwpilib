// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import java.util.Set;
import java.util.function.Supplier;

/**
 * A robot subsystem. Subsystems are the basic unit of robot organization in the Command-based
 * framework; they encapsulate low-level hardware objects (motor controllers, sensors, etc.) and
 * provide methods through which they can be used by {@link Command}s. Subsystems are used by the
 * {@link CommandScheduler}'s resource management system to ensure multiple robot actions are not
 * "fighting" over the same hardware; Commands that use a subsystem should include that subsystem in
 * their {@link Command#getRequirements()} method, and resources used within a subsystem should
 * generally remain encapsulated and not be shared by other parts of the robot.
 *
 * <p>Subsystems must be registered with the scheduler with the {@link
 * CommandScheduler#registerSubsystem(Subsystem...)} method in order for the {@link
 * Subsystem#periodic()} method to be called. It is recommended that this method be called from the
 * constructor of users' Subsystem implementations. The {@link SubsystemBase} class offers a simple
 * base for user implementations that handles this.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public interface Subsystem {
  /**
   * This method is called periodically by the {@link CommandScheduler}. Useful for updating
   * subsystem-specific state that you don't want to offload to a {@link Command}. Teams should try
   * to be consistent within their own codebases about which responsibilities will be handled by
   * Commands, and which will be handled here.
   */
  default void periodic() {}

  /**
   * This method is called periodically by the {@link CommandScheduler}. Useful for updating
   * subsystem-specific state that needs to be maintained for simulations, such as for updating
   * {@link edu.wpi.first.wpilibj.simulation} classes and setting simulated sensor readings.
   */
  default void simulationPeriodic() {}

  /**
   * Gets the subsystem name of this Subsystem.
   *
   * @return Subsystem name
   */
  default String getName() {
    return this.getClass().getSimpleName();
  }

  /**
   * Sets the default {@link Command} of the subsystem. The default command will be automatically
   * scheduled when no other commands are scheduled that require the subsystem. Default commands
   * should generally not end on their own, i.e. their {@link Command#isFinished()} method should
   * always return false. Will automatically register this subsystem with the {@link
   * CommandScheduler}.
   *
   * @param defaultCommand the default command to associate with this subsystem
   */
  default void setDefaultCommand(Command defaultCommand) {
    CommandScheduler.getInstance().setDefaultCommand(this, defaultCommand);
  }

  /**
   * Removes the default command for the subsystem. This will not cancel the default command if it
   * is currently running.
   */
  default void removeDefaultCommand() {
    CommandScheduler.getInstance().removeDefaultCommand(this);
  }

  /**
   * Gets the default command for this subsystem. Returns null if no default command is currently
   * associated with the subsystem.
   *
   * @return the default command associated with this subsystem
   */
  default Command getDefaultCommand() {
    return CommandScheduler.getInstance().getDefaultCommand(this);
  }

  /**
   * Returns the command currently running on this subsystem. Returns null if no command is
   * currently scheduled that requires this subsystem.
   *
   * @return the scheduled command currently requiring this subsystem
   */
  default Command getCurrentCommand() {
    return CommandScheduler.getInstance().requiring(this);
  }

  /**
   * Registers this subsystem with the {@link CommandScheduler}, allowing its {@link
   * Subsystem#periodic()} method to be called when the scheduler runs.
   */
  default void register() {
    CommandScheduler.getInstance().registerSubsystem(this);
  }

  /**
   * Constructs a command that runs an action once and finishes. Requires this subsystem.
   *
   * @param action the action to run
   * @return the command
   * @see InstantCommand
   */
  default Command runOnce(Runnable action) {
    return Commands.runOnce(action, this);
  }

  /**
   * Constructs a command that runs an action every iteration until interrupted. Requires this
   * subsystem.
   *
   * @param action the action to run
   * @return the command
   * @see RunCommand
   */
  default Command run(Runnable action) {
    return Commands.run(action, this);
  }

  /**
   * Constructs a command that runs an action once and another action when the command is
   * interrupted. Requires this subsystem.
   *
   * @param start the action to run on start
   * @param end the action to run on interrupt
   * @return the command
   * @see StartEndCommand
   */
  default Command startEnd(Runnable start, Runnable end) {
    return Commands.startEnd(start, end, this);
  }

  /**
   * Constructs a command that runs an action every iteration until interrupted, and then runs a
   * second action. Requires this subsystem.
   *
   * @param run the action to run every iteration
   * @param end the action to run on interrupt
   * @return the command
   */
  default Command runEnd(Runnable run, Runnable end) {
    return Commands.runEnd(run, end, this);
  }

  /**
   * Constructs a command that runs an action once and then runs another action every iteration
   * until interrupted. Requires this subsystem.
   *
   * @param start the action to run on start
   * @param run the action to run every iteration
   * @return the command
   */
  default Command startRun(Runnable start, Runnable run) {
    return Commands.startRun(start, run, this);
  }

  /**
   * Constructs a {@link DeferredCommand} with the provided supplier. This subsystem is added as a
   * requirement.
   *
   * @param supplier the command supplier.
   * @return the command.
   * @see DeferredCommand
   */
  default Command defer(Supplier<Command> supplier) {
    return Commands.defer(supplier, Set.of(this));
  }
}
