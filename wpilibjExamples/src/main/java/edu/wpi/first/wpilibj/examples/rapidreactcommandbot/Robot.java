// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot;

import edu.wpi.first.epilogue.Epilogue;
import edu.wpi.first.epilogue.Logged;
import edu.wpi.first.wpilibj.DataLogManager;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.AutoConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.OIConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.ShooterConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems.Drive;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems.Intake;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems.Pneumatics;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems.Shooter;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems.Storage;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandRobot;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.SendableChooserCommand;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;

/**
 * This class automatically runs the {@link CommandScheduler} for you. If you change the name of
 * this class or the package after creating this project, you must also update the Main.java file in
 * the project.
 */
@Logged(name = "Rapid React Command Robot")
public class Robot extends CommandRobot {
  // The robot's subsystems
  private final Drive m_drive = new Drive();
  private final Intake m_intake = new Intake();
  private final Storage m_storage = new Storage();
  private final Shooter m_shooter = new Shooter();
  private final Pneumatics m_pneumatics = new Pneumatics();

  // The driver's controller
  CommandXboxController m_driverController =
      new CommandXboxController(OIConstants.kDriverControllerPort);

  private final SendableChooser<Command> m_autoChooser = new SendableChooser<>();

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  public Robot() {
    // Automatically run the storage motor whenever the ball storage is not full,
    // and turn it off whenever it fills. Uses subsystem-hosted trigger to
    // improve readability and make inter-subsystem communication easier.
    m_storage.hasCargo.whileFalse(m_storage.runCommand());

    // Automatically disable and retract the intake whenever the ball storage is
    // full.
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
            Commands.parallel(
                    m_shooter.shootCommand(ShooterConstants.kShooterTargetRPS),
                    m_storage.runCommand())
                // Since we composed this inline we should give it a name
                .withName("Shoot"));

    // Toggle compressor with the Start button
    m_driverController.start().toggleOnTrue(m_pneumatics.disableCompressorCommand());

    // Initialize data logging.
    DataLogManager.start();
    Epilogue.bind(this);

    m_autoChooser.setDefaultOption(
        "Simple Auto",
        m_drive
            .driveDistanceCommand(AutoConstants.kDriveDistanceMeters, AutoConstants.kDriveSpeed)
            .withTimeout(AutoConstants.kTimeoutSeconds));

    m_autonomous.whileTrue(new SendableChooserCommand(m_autoChooser));
  }
}
