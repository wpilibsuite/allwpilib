// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.expansionhubsample;

import org.wpilib.driverstation.DefaultUserControls;
import org.wpilib.driverstation.UserControlsInstance;
import org.wpilib.framework.OpModeRobot;
import org.wpilib.hardware.expansionhub.ExpansionHubMotor;
import org.wpilib.hardware.expansionhub.ExpansionHubServo;

/**
 * This is a demo program showing the use of the DifferentialDrive class. Runs the motors with split
 * arcade steering and a gamepad.
 */
@UserControlsInstance(DefaultUserControls.class)
public class Robot extends OpModeRobot {
  public final ExpansionHubMotor leftMotor = new ExpansionHubMotor(0, 0);
  public final ExpansionHubMotor rightMotor = new ExpansionHubMotor(0, 1);
  public final ExpansionHubMotor frontMotor = new ExpansionHubMotor(0, 2);
  public final ExpansionHubMotor backMotor = new ExpansionHubMotor(0, 3);

  public final ExpansionHubServo servo = new ExpansionHubServo(0, 0);
  public final ExpansionHubServo servo2 = new ExpansionHubServo(0, 1);

  /** Called once at the beginning of the robot program. */
  public Robot() {
  }
}
