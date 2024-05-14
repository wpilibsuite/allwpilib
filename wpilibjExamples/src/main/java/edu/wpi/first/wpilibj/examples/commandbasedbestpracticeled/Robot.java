// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Example program demonstrating:
 * 
 * Splitting a common resource into two separately used resources (LEDs)
 * Configure button trigger
 * Triggers
 * Use of command parameters set at command creation time
 * Use of command parameters set at changable at runtime (Suppliers)
 * Use of method reference
 * Inject TimedRobot.addPeriodic() into other classes
 * Some commentary on composite commands and mode changes
 * Command logging
 * Configuring an autonomous commnad
 * Use of Xbox controller to produce fake events
 * Use of Xbox controller to trigger an event
 * Use of public command factories in subsystems
 * Use of private non-Command methods to prevent other classes from forgetting to lock a subsystem
 * Change TimeRobot loop speed
 * Change LED update rate different from the TimedRobot loop speed
 * Overloading method parameter types
 * 
 * There is some unused code and commented out code anticipating extensions to the WPILib addressable LED class.
 * This example program runs in real or simulated mode of the 2024 WPILib.
 * 
 * This is a refactor and modest extension of code donated by ChiefDelphi @illinar.
 * Any errors or confusions are the fault and responsibility of ChiefDelphi @SLAB-Mr.Thomas.
 */
package frc.robot;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;

public class Robot extends TimedRobot {

  private RobotContainer m_robotContainer;
  private Command m_autonomousCommand;

  Robot() {
    super(1.); // slow things down to human speed
  }

  @Override
  public void robotInit() {
    m_robotContainer = new RobotContainer(this::addPeriodic);
  }

  @Override
  public void robotPeriodic() {
    CommandScheduler.getInstance().run();
  }

  @Override
  public void disabledInit() {} // commands running from another mode haven't been cancelled

  @Override
  public void disabledPeriodic() {}

  @Override
  public void disabledExit() {}

  @Override
  public void autonomousInit() { // commands running from another mode haven't been cancelled
    m_autonomousCommand = m_robotContainer.getAutonomousCommand();

    if (m_autonomousCommand != null) {
      m_autonomousCommand.schedule();
    }
  }

  @Override
  public void autonomousPeriodic() {}

  @Override
  public void autonomousExit() {}

  @Override
  public void teleopInit() { // commands running from another mode haven't been cancelled except the one below
    if (m_autonomousCommand != null) {
      m_autonomousCommand.cancel();
    }
  }

  @Override
  public void teleopPeriodic() {}

  @Override
  public void teleopExit() {}

  @Override
  public void testInit() {
    CommandScheduler.getInstance().cancelAll();
  }

  @Override
  public void testPeriodic() {}

  @Override
  public void testExit() {}
}