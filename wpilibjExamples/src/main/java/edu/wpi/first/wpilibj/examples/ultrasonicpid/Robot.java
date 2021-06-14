// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.ultrasonicpid;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.filter.MedianFilter;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

/**
 * This is a sample program to demonstrate the use of a PIDController with an ultrasonic sensor to
 * reach and maintain a set distance from an object.
 */
public class Robot extends TimedRobot {
  // distance in inches the robot wants to stay from an object
  private static final double kHoldDistance = 12.0;

  // factor to convert sensor values to a distance in inches
  private static final double kValueToInches = 0.125;

  // proportional speed constant
  private static final double kP = 7.0;

  // integral speed constant
  private static final double kI = 0.018;

  // derivative speed constant
  private static final double kD = 1.5;

  private static final int kLeftMotorPort = 0;
  private static final int kRightMotorPort = 1;
  private static final int kUltrasonicPort = 0;

  // median filter to discard outliers; filters over 5 samples
  private final MedianFilter m_filter = new MedianFilter(5);

  private final AnalogInput m_ultrasonic = new AnalogInput(kUltrasonicPort);
  private final DifferentialDrive m_robotDrive =
      new DifferentialDrive(new PWMSparkMax(kLeftMotorPort), new PWMSparkMax(kRightMotorPort));
  private final PIDController m_pidController = new PIDController(kP, kI, kD);

  @Override
  public void teleopInit() {
    // Set setpoint of the pid controller
    m_pidController.setSetpoint(kHoldDistance * kValueToInches);
  }

  @Override
  public void teleopPeriodic() {
    // returned value is filtered with a rolling median filter, since ultrasonics
    // tend to be quite noisy and susceptible to sudden outliers
    double pidOutput = m_pidController.calculate(m_filter.calculate(m_ultrasonic.getVoltage()));

    m_robotDrive.arcadeDrive(pidOutput, 0);
  }
}
