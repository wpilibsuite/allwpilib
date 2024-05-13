package edu.wpi.first.wpilibj2.command;

/**
 * NOTE: The `Subsystem` interface has been renamed to {@link Resource}; this interface remains as
 * an alias for backwards-compatibility.  The new name better represents the in-code purpose of
 * the interface.
 *
 * <p>A robot subsystem. Subsystems are the basic unit of robot organization in the Command-based
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
public interface Subsystem extends Resource {
}
