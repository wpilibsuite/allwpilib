// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.wpilibj.DutyCycleEncoder;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

public class MyRobot extends TimedRobot {
  DutyCycleEncoder pot;
  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  @Override
  public void robotInit() {
    pot = new DutyCycleEncoder(0, 3, 0.5);
    pot.setAssumedFrequency(967.77);
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

  /** This function is called periodically during test mode. */
  @Override
  public void testPeriodic() {}

  /** This function is called periodically during all modes. */
  @Override
  public void robotPeriodic() {
    double val = pot.get() + 1;
    SmartDashboard.putNumber("DC4", val);
    SmartDashboard.putNumber("DC3", MathUtil.invertInput(val, 1, 4));
    SmartDashboard.putNumber("DC2", MathUtil.inputModulus(pot.get(), -0.1, 0.9));
    SmartDashboard.putNumber("DC", MathUtil.inputModulus(MathUtil.invertInput(pot.get(), 0, 1), -0.1, 0.9));
    SmartDashboard.putNumber("Freq", pot.getFrequency());
  }
}
