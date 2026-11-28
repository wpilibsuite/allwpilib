// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbotinlined;

import org.wpilib.command2.Command;
import org.wpilib.command2.CommandScheduler;
import org.wpilib.command2.Commands;
import org.wpilib.command2.button.CommandGamepad;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.examples.hatchbotinlined.Constants.OIConstants;
import org.wpilib.examples.hatchbotinlined.commands.Autos;
import org.wpilib.examples.hatchbotinlined.subsystems.DriveSubsystem;
import org.wpilib.examples.hatchbotinlined.subsystems.HatchSubsystem;
import org.wpilib.framework.TimedRobot;
import org.wpilib.system.DataLogManager;
import org.wpilib.telemetry.Telemetry;
import org.wpilib.tunable.Selectable;
import org.wpilib.tunable.Tunables;

/**
 * The methods in this class are called automatically corresponding to each mode, as described in
 * the TimedRobot documentation. If you change the name of this class or the package after creating
 * this project, you must also update the Main.java file in the project.
 */
public class Robot extends TimedRobot {
  private Command autonomousCommand;

  // The robot's subsystems
  private final DriveSubsystem robotDrive = new DriveSubsystem();
  private final HatchSubsystem hatchSubsystem = new HatchSubsystem();

  // Retained command handles

  // The autonomous routines
  // A simple auto routine that drives forward a specified distance, and then stops.
  private final Command simpleAuto = Autos.simpleAuto(robotDrive);
  // A complex auto routine that drives forward, drops a hatch, and then drives backward.
  private final Command complexAuto = Autos.complexAuto(robotDrive, hatchSubsystem);

  // A chooser for autonomous commands
  private final Selectable<Command> chooser = new Selectable<>();

  // The driver's controller
  CommandGamepad driverController = new CommandGamepad(OIConstants.DRIVER_CONTROLLER_PORT);

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  public Robot() {
    // Configure the button bindings
    configureButtonBindings();

    // Configure default commands
    // Set the default drive command to split-stick arcade drive
    robotDrive.setDefaultCommand(
        // A split-stick arcade command, with forward/backward controlled by the left
        // hand, and turning controlled by the right.
        Commands.run(
            () ->
                robotDrive.arcadeDrive(-driverController.getLeftY(), -driverController.getRightX()),
            robotDrive));

    // Add commands to the autonomous command chooser
    chooser.addDefault("Simple Auto", simpleAuto);
    chooser.add("Complex Auto", complexAuto);

    // Put the chooser on the dashboard
    Tunables.publish("Autonomous", chooser);

    // Start recording to data log
    DataLogManager.start();

    // Record DS control and joystick data.
    // Change to `false` to not record joystick data.
    DriverStation.startDataLog(DataLogManager.getLog(), true);
  }

  /**
   * Use this method to define your button->command mappings. Buttons can be created by
   * instantiating a {@link org.wpilib.driverstation.GenericHID} or one of its subclasses ({@link
   * org.wpilib.driverstation.Joystick} or {@link Gamepad}), and then passing it to a {@link
   * org.wpilib.command2.button.JoystickButton}.
   */
  private void configureButtonBindings() {
    // Grab the hatch when the right face button is pressed.
    driverController.faceRight().onTrue(hatchSubsystem.grabHatchCommand());
    // Release the hatch when the left face button is pressed.
    driverController.faceLeft().onTrue(hatchSubsystem.releaseHatchCommand());
    // While holding right bumper, drive at half velocity
    driverController
        .rightBumper()
        .onTrue(Commands.runOnce(() -> robotDrive.setMaxOutput(0.5)))
        .onFalse(Commands.runOnce(() -> robotDrive.setMaxOutput(1)));
  }

  /**
   * This function is called every 20 ms, no matter the mode. Use this for items like diagnostics
   * that you want ran during disabled, autonomous, teleoperated and utility.
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

    // Put subsystems to telemetry.
    Telemetry.log("Drivetrain", robotDrive);
    Telemetry.log("HatchSubsystem", hatchSubsystem);
  }

  /** This function is called each time Disabled mode starts. */
  @Override
  public void disabledStart() {}

  @Override
  public void disabledPeriodic() {}

  /** This autonomous runs the autonomous command selected by the {@link #chooser}. */
  @Override
  public void autonomousStart() {
    autonomousCommand = chooser.getSelected();

    // schedule the autonomous command (example)
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
