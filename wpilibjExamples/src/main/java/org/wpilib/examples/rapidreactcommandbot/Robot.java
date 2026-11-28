// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rapidreactcommandbot;

import static org.wpilib.command2.Commands.parallel;

import org.wpilib.command2.Command;
import org.wpilib.command2.CommandScheduler;
import org.wpilib.command2.button.CommandGamepad;
import org.wpilib.command2.button.Trigger;
import org.wpilib.epilogue.Epilogue;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rapidreactcommandbot.Constants.AutoConstants;
import org.wpilib.examples.rapidreactcommandbot.Constants.OIConstants;
import org.wpilib.examples.rapidreactcommandbot.Constants.ShooterConstants;
import org.wpilib.examples.rapidreactcommandbot.subsystems.Drive;
import org.wpilib.examples.rapidreactcommandbot.subsystems.Intake;
import org.wpilib.examples.rapidreactcommandbot.subsystems.Pneumatics;
import org.wpilib.examples.rapidreactcommandbot.subsystems.Shooter;
import org.wpilib.examples.rapidreactcommandbot.subsystems.Storage;
import org.wpilib.framework.TimedRobot;
import org.wpilib.system.DataLogManager;

/**
 * The methods in this class are called automatically corresponding to each mode, as described in
 * the TimedRobot documentation. If you change the name of this class or the package after creating
 * this project, you must also update the Main.java file in the project.
 */
@Logged(name = "Rapid React Command Robot")
public class Robot extends TimedRobot {
  private Command autonomousCommand;

  // The robot's subsystems
  private final Drive drive = new Drive();
  private final Intake intake = new Intake();
  private final Storage storage = new Storage();
  private final Shooter shooter = new Shooter();
  private final Pneumatics pneumatics = new Pneumatics();

  // The driver's controller
  CommandGamepad driverController = new CommandGamepad(OIConstants.DRIVER_CONTROLLER_PORT);

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  public Robot() {
    // Configure default commands and condition bindings on robot startup
    configureBindings();

    // Initialize data logging.
    DataLogManager.start();
    Epilogue.bind(this);
  }

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

    // Deploy the intake with the left face button
    driverController.faceLeft().onTrue(intake.intakeCommand());
    // Retract the intake with the up face button
    driverController.faceUp().onTrue(intake.retractCommand());

    // Fire the shooter with the down face button
    driverController
        .faceDown()
        .onTrue(
            parallel(
                    shooter.shootCommand(ShooterConstants.SHOOTER_TARGET_RPS), storage.runCommand())
                // Since we composed this inline we should give it a name
                .withName("Shoot"));

    // Toggle compressor with the Start button
    driverController.start().toggleOnTrue(pneumatics.disableCompressorCommand());
  }

  /**
   * This function is called every robot packet, no matter the mode. Use this for items like
   * diagnostics that you want ran during disabled, autonomous, teleoperated and utility.
   *
   * <p>This runs after the mode specific periodic functions, but before LiveWindow and
   * SmartDashboard integrated updating.
   */
  @Override
  public void robotPeriodic() {
    // Runs the Scheduler.  This is responsible for polling buttons, adding newly-scheduled
    // commands, running already-scheduled commands, removing finished or interrupted commands,
    // and running subsystem periodic() methods.  This must be called from the robot's periodic
    // block in order for anything in the Command-based framework to work.
    CommandScheduler.getInstance().run();
  }

  /** This function is called each time Disabled mode starts. */
  @Override
  public void disabledStart() {}

  @Override
  public void disabledPeriodic() {}

  @Override
  public void autonomousStart() {
    // Drive forward for 2 meters at half velocity with a 3 second timeout
    autonomousCommand =
        drive
            .driveDistanceCommand(AutoConstants.DRIVE_DISTANCE, AutoConstants.DRIVE_VELOCITY)
            .withTimeout(AutoConstants.TIMEOUT);
    CommandScheduler.getInstance().schedule(autonomousCommand);
  }

  /** This function is called periodically during autonomous. */
  @Override
  public void autonomousPeriodic() {}

  @Override
  public void autonomousEnd() {
    // This makes sure that the autonomous stops running when
    // autonomous mode ends. If you want the autonomous to
    // continue until interrupted by another command, remove
    // this line or comment it out.
    if (autonomousCommand != null) {
      autonomousCommand.cancel();
    }
  }

  /** This function is called each time teleoperated mode starts. */
  @Override
  public void teleopStart() {}

  /** This function is called periodically during teleoperated mode. */
  @Override
  public void teleopPeriodic() {}

  @Override
  public void utilityStart() {
    // Cancels all running commands at the start of utility mode.
    CommandScheduler.getInstance().cancelAll();
  }

  /** This function is called periodically during utility mode. */
  @Override
  public void utilityPeriodic() {}
}
