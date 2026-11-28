// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.xrpreference;

import org.wpilib.command2.Command;
import org.wpilib.command2.CommandScheduler;
import org.wpilib.command2.InstantCommand;
import org.wpilib.command2.PrintCommand;
import org.wpilib.command2.button.JoystickButton;
import org.wpilib.command2.button.Trigger;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.driverstation.GenericHID;
import org.wpilib.driverstation.Joystick;
import org.wpilib.examples.xrpreference.commands.ArcadeDrive;
import org.wpilib.examples.xrpreference.commands.AutonomousDistance;
import org.wpilib.examples.xrpreference.commands.AutonomousTime;
import org.wpilib.examples.xrpreference.subsystems.Arm;
import org.wpilib.examples.xrpreference.subsystems.Drivetrain;
import org.wpilib.framework.TimedRobot;
import org.wpilib.tunable.Selectable;
import org.wpilib.tunable.Tunables;
import org.wpilib.xrp.XRPOnBoardIO;

/**
 * The methods in this class are called automatically corresponding to each mode, as described in
 * the TimedRobot documentation. If you change the name of this class or the package after creating
 * this project, you must also update the Main.java file in the project.
 */
public class Robot extends TimedRobot {
  private Command autonomousCommand;

  private final Drivetrain drivetrain = new Drivetrain();
  private final XRPOnBoardIO onboardIO = new XRPOnBoardIO();
  private final Arm arm = new Arm();

  // Assumes a gamepad plugged into channel 0
  private final Joystick controller = new Joystick(0);

  // Create chooser for autonomous routines
  private final Selectable<Command> chooser = new Selectable<>();

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  public Robot() {
    // Configure the button bindings
    configureButtonBindings();
  }

  /**
   * Use this method to define your button->command mappings. Buttons can be created by
   * instantiating a {@link GenericHID} or one of its subclasses ({@link
   * org.wpilib.driverstation.Joystick} or {@link Gamepad}), and then passing it to a {@link
   * org.wpilib.command2.button.JoystickButton}.
   */
  private void configureButtonBindings() {
    // Default command is arcade drive. This will run unless another command
    // is scheduled over it.
    drivetrain.setDefaultCommand(
        new ArcadeDrive(
            drivetrain, () -> -controller.getRawAxis(1), () -> -controller.getRawAxis(2)));

    // Example of how to use the onboard IO
    Trigger userButton = new Trigger(onboardIO::getUserButtonPressed);
    userButton
        .onTrue(new PrintCommand("USER Button Pressed"))
        .onFalse(new PrintCommand("USER Button Released"));

    JoystickButton joystickAButton = new JoystickButton(controller, 1);
    joystickAButton
        .onTrue(new InstantCommand(() -> arm.setAngle(45.0), arm))
        .onFalse(new InstantCommand(() -> arm.setAngle(0.0), arm));

    JoystickButton joystickBButton = new JoystickButton(controller, 2);
    joystickBButton
        .onTrue(new InstantCommand(() -> arm.setAngle(90.0), arm))
        .onFalse(new InstantCommand(() -> arm.setAngle(0.0), arm));

    // Setup SmartDashboard options
    chooser.addDefault("Auto Routine Distance", new AutonomousDistance(drivetrain));
    chooser.add("Auto Routine Time", new AutonomousTime(drivetrain));
    Tunables.publish("Auto Chooser", chooser);
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

  /** This function is called once when the robot is first started up. */
  @Override
  public void simulationInit() {}

  /** This function is called periodically whilst in simulation. */
  @Override
  public void simulationPeriodic() {}
}
