// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbottraditional;

import static org.wpilib.driverstation.Gamepad.Button;

import org.wpilib.command2.Command;
import org.wpilib.command2.CommandScheduler;
import org.wpilib.command2.button.GamepadButton;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.examples.hatchbottraditional.Constants.AutoConstants;
import org.wpilib.examples.hatchbottraditional.Constants.OIConstants;
import org.wpilib.examples.hatchbottraditional.commands.ComplexAuto;
import org.wpilib.examples.hatchbottraditional.commands.DefaultDrive;
import org.wpilib.examples.hatchbottraditional.commands.DriveDistance;
import org.wpilib.examples.hatchbottraditional.commands.GrabHatch;
import org.wpilib.examples.hatchbottraditional.commands.HalveDriveVelocity;
import org.wpilib.examples.hatchbottraditional.commands.ReleaseHatch;
import org.wpilib.examples.hatchbottraditional.subsystems.DriveSubsystem;
import org.wpilib.examples.hatchbottraditional.subsystems.HatchSubsystem;
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

  // The autonomous routines

  // A simple auto routine that drives forward a specified distance, and then stops.
  private final Command simpleAuto =
      new DriveDistance(
          AutoConstants.AUTO_DRIVE_DISTANCE_INCHES, AutoConstants.AUTO_DRIVE_VELOCITY, robotDrive);

  // A complex auto routine that drives forward, drops a hatch, and then drives backward.
  private final Command complexAuto = new ComplexAuto(robotDrive, hatchSubsystem);

  // A chooser for autonomous commands
  private final Selectable<Command> chooser = new Selectable<>();

  // The driver's controller
  Gamepad driverController = new Gamepad(OIConstants.DRIVER_CONTROLLER_PORT);

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
        new DefaultDrive(
            robotDrive, () -> -driverController.getLeftY(), () -> -driverController.getRightX()));

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
    // Grab the hatch when the 'Face Down' button is pressed.
    new GamepadButton(driverController, Button.FACE_DOWN).onTrue(new GrabHatch(hatchSubsystem));
    // Release the hatch when the 'Face Right' button is pressed.
    new GamepadButton(driverController, Button.FACE_RIGHT).onTrue(new ReleaseHatch(hatchSubsystem));
    // While holding the bumper button, drive at half velocity
    new GamepadButton(driverController, Button.RIGHT_BUMPER)
        .whileTrue(new HalveDriveVelocity(robotDrive));
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

    // Put telemetry.
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

    /*
     * String autoSelected = SmartDashboard.getString("Auto Selector",
     * "Default"); switch(autoSelected) { case "My Auto": autonomousCommand
     * = new MyAutoCommand(); break; case "Default Auto": default:
     * autonomousCommand = new ExampleCommand(); break; }
     */

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
