// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj2.command.button.Trigger;

/**
 * A robot subsystem. Subsystems are the basic unit of robot organization in the Command-based
 * framework; they encapsulate low-level hardware objects (motor controllers, sensors, etc) and
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
   * Sets the default {@link Command} of the subsystem. The default command will be automatically
   * scheduled when no other commands are scheduled that require the subsystem. Will automatically
   * register this subsystem with the {@link CommandScheduler}.
   *
   * @param defaultCommand the default command to associate with this subsystem
   * @deprecated replace with an explicit Trigger binding:<code>
   *   subsystem.requiredTrigger().onFalse(defaultCommand)
   * </code>
   * @see #requiredTrigger(EventLoop)
   */
  @Deprecated
  default void setDefaultCommand(Command defaultCommand) {
    CommandScheduler.getInstance().setDefaultCommand(this, defaultCommand);
  }

  /**
   * Gets the default command for this subsystem. Returns null if no default command is currently
   * associated with the subsystem.
   *
   * @return the default command associated with this subsystem
   * @deprecated this will return only default commands set through {@link
   *     #setDefaultCommand(Command)}.
   */
  @Deprecated
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
   * Returns `true` if the subsystem is currently required by a command.
   *
   * @return true if required
   */
  default boolean isRequired() {
    return CommandScheduler.getInstance().isRequired(this);
  }

  /**
   * Returns a Trigger that is `true` when this subsystem is required. Useful for binding a "default
   * command" that will start when no commands require this subsystem. Bound to the {@link
   * CommandScheduler#getDefaultButtonLoop() default button loop}.
   *
   * @return a Trigger
   */
  default Trigger requiredTrigger() {
    return requiredTrigger(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Returns a Trigger that is `true` when this subsystem is required. Useful for binding a "default
   * command" that will start when no commands require this subsystem.
   *
   * @param loop the EventLoop to bind the Trigger to. Defaults to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default button loop}.
   * @return a Trigger
   */
  default Trigger requiredTrigger(EventLoop loop) {
    return new Trigger(loop, this::isRequired);
  }

  /**
   * Registers this subsystem with the {@link CommandScheduler}, allowing its {@link
   * Subsystem#periodic()} method to be called when the scheduler runs.
   */
  default void register() {
    CommandScheduler.getInstance().registerSubsystem(this);
  }
}
