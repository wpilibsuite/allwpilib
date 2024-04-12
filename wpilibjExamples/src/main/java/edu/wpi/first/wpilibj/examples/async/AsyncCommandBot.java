// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.async;

import edu.wpi.first.wpilibj.examples.async.Constants.AutoConstants;
import edu.wpi.first.wpilibj.examples.async.Constants.OIConstants;
import edu.wpi.first.wpilibj.examples.async.Constants.ShooterConstants;
import edu.wpi.first.wpilibj.examples.async.subsystems.Drive;
import edu.wpi.first.wpilibj.examples.async.subsystems.Intake;
import edu.wpi.first.wpilibj.examples.async.subsystems.Pneumatics;
import edu.wpi.first.wpilibj.examples.async.subsystems.Shooter;
import edu.wpi.first.wpilibj.examples.async.subsystems.Storage;
import edu.wpi.first.wpilibj2.command.button.Trigger;
import edu.wpi.first.wpilibj3.command.async.AsyncCommand;
import edu.wpi.first.wpilibj3.command.async.ParallelGroup;
import edu.wpi.first.wpilibj3.command.async.button.AsyncCommandXboxController;

/**
 * This class is where the bulk of the robot should be declared. Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
 * subsystems, commands, and button mappings) should be declared here.
 */
public class AsyncCommandBot {
  // The robot's subsystems
  private final Drive m_drive = new Drive();
  private final Intake m_intake = new Intake();
  private final Storage m_storage = new Storage();
  private final Shooter m_shooter = new Shooter();
  private final Pneumatics m_pneumatics = new Pneumatics();

  // The driver's controller
  private final AsyncCommandXboxController m_driverController =
      new AsyncCommandXboxController(OIConstants.kDriverControllerPort);

  /**
   * Use this method to define bindings between conditions and commands. These are useful for
   * automating robot behaviors based on button and sensor input.
   *
   * <p>Should be called during {@link Robot#robotInit()}.
   *
   * <p>Event binding methods are available on the {@link Trigger} class.
   */
  public void configureBindings() {
    // Automatically run the storage motor whenever the ball storage is not full,
    // and turn it off whenever it fills. Uses subsystem-hosted trigger to
    // improve readability and make inter-subsystem communication easier.
    m_storage.hasCargo.whileFalse(m_storage.runCommand());

    // Automatically disable and retract the intake whenever the ball storage is full.
    m_storage.hasCargo.onTrue(m_intake.retractCommand());

    // Control the drive with split-stick arcade controls
    m_drive.setDefaultCommand(
        m_drive.arcadeDriveCommand(
            () -> -m_driverController.getLeftY(), () -> -m_driverController.getRightX()));

    // Deploy the intake with the X button
    m_driverController.x().onTrue(m_intake.intakeCommand());
    // Retract the intake with the Y button
    m_driverController.y().onTrue(m_intake.retractCommand());

    // Fire the shooter with the A button
    m_driverController
        .a()
        .onTrue(
            ParallelGroup.onDefaultScheduler().all(
                    m_shooter.shootCommand(ShooterConstants.kShooterTargetRPS),
                    m_storage.runCommand())
                .named("Shoot"));

    // Toggle compressor with the Start button
    m_driverController.start().toggleOnTrue(m_pneumatics.disableCompressorCommand());
  }

  /**
   * Use this to define the command that runs during autonomous.
   *
   * <p>Scheduled during {@link Robot#autonomousInit()}.
   */
  public AsyncCommand getAutonomousCommand() {
    // Drive forward for 2 meters at half speed with a 3 second timeout
    return ParallelGroup
             .onDefaultScheduler()
             .all(m_drive.driveDistanceCommand(AutoConstants.kDriveDistance, AutoConstants.kDriveSpeed))
             .withTimeout(AutoConstants.kTimeout)
             .named("Drive Across Starting Line");
  }
}
