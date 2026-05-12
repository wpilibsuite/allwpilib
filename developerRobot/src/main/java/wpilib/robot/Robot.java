// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.driverstation.Gamepad;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.expansionhub.ExpansionHubMotor;
import org.wpilib.hardware.expansionhub.ExpansionHubMotor.FollowDirection;

public class Robot extends TimedRobot {
  ExpansionHubMotor motor = new ExpansionHubMotor(0, 0);
  ExpansionHubMotor motor2 = new ExpansionHubMotor(0, 1);
  ExpansionHubMotor motor3 = new ExpansionHubMotor(0, 2);
  Gamepad gamepad = new Gamepad(0);

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  public Robot() {
    motor2.follow(motor3, FollowDirection.Opposed);
    motor3.follow(motor, FollowDirection.Opposed);
  }

  /** This function is run once each time the robot enters autonomous mode. */
  @Override
  public void autonomousInit() {}

  /** This function is called periodically during autonomous. */
  @Override
  public void autonomousPeriodic() {}

  /** This function is called once each time the robot enters tele-operated mode. */
  @Override
  public void teleopInit() {}

  /** This function is called periodically during operator control. */
  @Override
  public void teleopPeriodic() {}

  /** This function is called periodically during utility mode. */
  @Override
  public void utilityPeriodic() {}

  /** This function is called periodically during all modes. */
  @Override
  public void robotPeriodic() {
    motor.setThrottle(gamepad.getLeftY());
  }
}
