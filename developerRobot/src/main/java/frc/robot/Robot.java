// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.hal.PWMJNI;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.PWM;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.motorcontrol.SparkMini;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

public class Robot extends TimedRobot {
  DigitalInput di = new DigitalInput(1);
  //DigitalOutput dio = new DigitalOutput(0);
  SparkMini spark = new SparkMini(2);
  SparkMini spark2 = new SparkMini(3);
  SparkMini spark3 = new SparkMini(4);
  PWM pwm = new PWM(0);

  XboxController xbox = new XboxController(0);

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  public Robot() {
    PWMJNI.setPWMPeriodScale(spark2.getPwmHandle(), 2);
    PWMJNI.setPWMPeriodScale(spark3.getPwmHandle(), 3);

    pwm.setAlwaysHighMode();
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

  int count = 0;
  boolean val = false;

  /** This function is called periodically during all modes. */
  @Override
  public void robotPeriodic() {
    SmartDashboard.putBoolean("Input", di.get());

    spark.set(xbox.getLeftX());
    spark2.set(xbox.getLeftX());
    spark3.set(xbox.getLeftX());

    // count++;
    // if (count >= 20) {
    //   count = 0;
    //   val = !val;
    //   dio.set(val);
    // }
  }
}
