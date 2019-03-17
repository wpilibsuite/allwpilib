package edu.wpi.first.wpilibj.experimental.command;

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
 * {@link CommandScheduler#registerSubsystem(Subsystem)} method in order for the
 * {@link Subsystem#periodic()} method to be called, and for the default command to be scheduled.
 * It is recommended that this method be called from the constructor of users' Subsystem
 * implementations.  The {@link SendableSubsystemBase} class offers a simple base for user
 * implementations that handles this.
 */
public interface Subsystem {

  /**
   * This method is called periodically by the {@link CommandScheduler}.  Useful for updating
   * subsystem-specific state that you don't want to offload to a {@link Command}.  Teams should
   * try to be consistent within their own codebases about which responsibilities will be handled
   * by Commands, and which will be handled here.
   */
  default void periodic() {}

  /**
   * Specifies the default {@link Command} of the subsystem.  The default command will be
   * automatically scheduled when no other commands are scheduled that require the subsystem.
   * Default commands should generally not end on their own, i.e. their {@link Command#isFinished()}
   * method should always return false.
   *
   * @return the default command of this subsystem
   */
  default Command getDefaultCommand() {
    return null;
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

}
