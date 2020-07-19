/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

/**
 * A robot subsystem.  Subsystems are the basic unit of robot organization in the Command-based
 * framework; they encapsulate low-level hardware objects (motor controllers, sensors, etc) and
 * provide methods through which they can be used by {@link Command}s.  Subsystems are used by the
 * {@link CommandScheduler}'s resource management system to ensure multiple robot actions are not
 * "fighting" over the same hardware; Commands that use a subsystem should include that subsystem
 * in their {@link Command#getRequirements()} method, and resources used within a subsystem should
 * generally remain encapsulated and not be shared by other parts of the robot.
 *
 * <p>Subsystems must be registered with the scheduler with the
 * {@link CommandScheduler#registerSubsystem(Subsystem...)} method in order for the
 * {@link Subsystem#periodic()} method to be called.  It is recommended that this method be called
 * from the constructor of users' Subsystem implementations.  The {@link SubsystemBase}
 * class offers a simple base for user implementations that handles this.
 */
public interface Subsystem {

  /**
   * This method is called periodically by the {@link CommandScheduler}.  Useful for updating
   * subsystem-specific state that you don't want to offload to a {@link Command}.  Teams should
   * try to be consistent within their own codebases about which responsibilities will be handled
   * by Commands, and which will be handled here.
   */
  default void periodic() {
  }

  /**
   * This method is called periodically by the {@link CommandScheduler}.  Useful for updating
   * subsystem-specific state that needs to be maintained for simulations, such as for updating
   * {@link edu.wpi.first.wpilibj.simulation} classes and setting simulated sensor readings.
   */
  default void simulationPeriodic() {
  }

  /**
   * Sets the default {@link Command} of the subsystem.  The default command will be
   * automatically scheduled when no other commands are scheduled that require the subsystem.
   * Default commands should generally not end on their own, i.e. their {@link Command#isFinished()}
   * method should always return false.  Will automatically register this subsystem with the
   * {@link CommandScheduler}.
   *
   * @param defaultCommand the default command to associate with this subsystem
   */
  default void setDefaultCommand(Command defaultCommand) {
    CommandScheduler.getInstance().setDefaultCommand(this, defaultCommand);
  }

  /**
   * Gets the default command for this subsystem.  Returns null if no default command is
   * currently associated with the subsystem.
   *
   * @return the default command associated with this subsystem
   */
  default Command getDefaultCommand() {
    return CommandScheduler.getInstance().getDefaultCommand(this);
  }

  /**
   * Returns the command currently running on this subsystem.  Returns null if no command is
   * currently scheduled that requires this subsystem.
   *
   * @return the scheduled command currently requiring this subsystem
   */
  default Command getCurrentCommand() {
    return CommandScheduler.getInstance().requiring(this);
  }

  /**
   * Registers this subsystem with the {@link CommandScheduler}, allowing its
   * {@link Subsystem#periodic()} method to be called when the scheduler runs.
   */
  default void register() {
    CommandScheduler.getInstance().registerSubsystem(this);
  }
}
