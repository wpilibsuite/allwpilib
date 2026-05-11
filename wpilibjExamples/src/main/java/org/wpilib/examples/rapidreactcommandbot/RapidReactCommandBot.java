// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rapidreactcommandbot;

import static org.wpilib.command2.Commands.parallel;

import org.wpilib.command2.Command;
import org.wpilib.command2.button.CommandGamepad;
import org.wpilib.command2.button.Trigger;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rapidreactcommandbot.Constants.AutoConstants;
import org.wpilib.examples.rapidreactcommandbot.Constants.OIConstants;
import org.wpilib.examples.rapidreactcommandbot.Constants.ShooterConstants;
import org.wpilib.examples.rapidreactcommandbot.subsystems.Drive;
import org.wpilib.examples.rapidreactcommandbot.subsystems.Intake;
import org.wpilib.examples.rapidreactcommandbot.subsystems.Pneumatics;
import org.wpilib.examples.rapidreactcommandbot.subsystems.Shooter;
import org.wpilib.examples.rapidreactcommandbot.subsystems.Storage;

/**
 * This class is where the bulk of the robot should be declared. Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
 * subsystems, commands, and button mappings) should be declared here.
 */
@Logged(name = "Rapid React Command Robot Container")
public class RapidReactCommandBot {
  // The robot's subsystems
  private final Drive drive = new Drive();
  private final Intake intake = new Intake();
  private final Storage storage = new Storage();
  private final Shooter shooter = new Shooter();
  private final Pneumatics pneumatics = new Pneumatics();

  // The driver's controller
  CommandGamepad driverController = new CommandGamepad(OIConstants.kDriverControllerPort);

  /**
   * Use this method to define bindings between conditions and commands. These are useful for
   * automating robot behaviors based on button and sensor input.
   *
   * <p>Should be called in the robot class constructor.
   *
   * <p>Event binding methods are available on the {@link Trigger} class.
   */
  public void configureBindings() {
    // Automatically run the storage motor whenever the ball storage is not full,
    // and turn it off whenever it fills. Uses subsystem-hosted trigger to
    // improve readability and make inter-subsystem communication easier.
    storage.hasCargo.whileFalse(storage.runCommand());

    // Automatically disable and retract the intake whenever the ball storage is full.
    storage.hasCargo.onTrue(intake.retractCommand());

    // Control the drive with split-stick arcade controls
    drive.setDefaultCommand(
        drive.arcadeDriveCommand(
            () -> -driverController.getLeftY(), () -> -driverController.getRightX()));

    // Deploy the intake with the west face button
    driverController.westFace().onTrue(intake.intakeCommand());
    // Retract the intake with the north face button
    driverController.northFace().onTrue(intake.retractCommand());

    // Fire the shooter with the south face button
    driverController
        .southFace()
        .onTrue(
            parallel(shooter.shootCommand(ShooterConstants.kShooterTargetRPS), storage.runCommand())
                // Since we composed this inline we should give it a name
                .withName("Shoot"));

    // Toggle compressor with the Start button
    driverController.start().toggleOnTrue(pneumatics.disableCompressorCommand());
  }

  /**
   * Use this to define the command that runs during autonomous.
   *
   * <p>Scheduled during {@link Robot#autonomousInit()}.
   */
  public Command getAutonomousCommand() {
    // Drive forward for 2 meters at half velocity with a 3 second timeout
    return drive
        .driveDistanceCommand(AutoConstants.kDriveDistance, AutoConstants.kDriveVelocity)
        .withTimeout(AutoConstants.kTimeout);
  }
}
