// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj2.command.button.Trigger;

/**
 * CommandRobot
 */
public class CommandRobot extends TimedRobot {

  protected CommandRobot(double period) {
    super(period);
  }

  @Override
  public void robotPeriodic() {
    CommandScheduler.getInstance().run();
  }

  @Override
  public void simulationPeriodic() {}

  @Override
  public void disabledPeriodic() {}

  @Override
  public void autonomousPeriodic() {}

  @Override
  public void teleopPeriodic() {}

  @Override
  public void testPeriodic() {}

  @Override
  public void testInit() {
    CommandScheduler.getInstance().cancelAll();
  }

  public Trigger autonomous() {
    return new Trigger(DriverStation::isAutonomous);
  }

  public Trigger teleop() {
    return new Trigger(DriverStation::isTeleop);
  }

  public Trigger test() {
    return new Trigger(DriverStation::isTest);
  }

  public Trigger simulation() {
    return new Trigger(TimedRobot::isSimulation);
  }
}
