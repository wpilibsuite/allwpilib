/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PWMVictorSPX;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.SpeedControllerGroup;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

import edu.wpi.first.wpilibj.examples.gearsbot.Robot;

import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.kLeftEncoderPorts;
import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.kLeftEncoderReversed;
import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.kLeftFrontPort;
import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.kLeftRearPort;
import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.kRangeFinderPort;
import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.kRealEncoderDistancePerPulse;
import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.kRightEncoderPorts;
import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.kRightEncoderReversed;
import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.kRightFrontPort;
import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.kRightRearPort;
import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.DriveConstants.kSimEncoderDistancePerPulse;

public class DriveTrain extends SubsystemBase {
  /**
   * The DriveTrain subsystem incorporates the sensors and actuators attached to the robots chassis.
   * These include four drive motors, a left and right encoder and a gyro.
   */
  private final SpeedController m_leftMotor =
      new SpeedControllerGroup(new PWMVictorSPX(kLeftFrontPort), new PWMVictorSPX(kLeftRearPort));
  private final SpeedController m_rightMotor =
      new SpeedControllerGroup(new PWMVictorSPX(kRightFrontPort), new PWMVictorSPX(kRightRearPort));

  private final DifferentialDrive m_drive = new DifferentialDrive(m_leftMotor, m_rightMotor);

  private final Encoder m_leftEncoder = new Encoder(
      kLeftEncoderPorts[0], kLeftEncoderPorts[1], kLeftEncoderReversed);
  private final Encoder m_rightEncoder = new Encoder(
      kRightEncoderPorts[0], kRightEncoderPorts[1], kRightEncoderReversed);
  private final AnalogInput m_rangefinder = new AnalogInput(kRangeFinderPort);
  private final AnalogGyro m_gyro = new AnalogGyro(1);

  /**
   * Create a new drive train subsystem.
   */
  public DriveTrain() {
    super();

    // Encoders may measure differently in the real world and in
    // simulation. In this example the robot moves 0.042 barleycorns
    // per tick in the real world, but the simulated encoders
    // simulate 360 tick encoders. This if statement allows for the
    // real robot to handle this difference in devices.
    if (Robot.isReal()) {
      m_leftEncoder.setDistancePerPulse(kRealEncoderDistancePerPulse);
      m_rightEncoder.setDistancePerPulse(kRealEncoderDistancePerPulse);
    } else {
      m_leftEncoder.setDistancePerPulse(kSimEncoderDistancePerPulse);
      m_rightEncoder.setDistancePerPulse(kSimEncoderDistancePerPulse);
    }

    // Let's name the sensors on the LiveWindow
    addChild("Drive", m_drive);
    addChild("Left Encoder", m_leftEncoder);
    addChild("Right Encoder", m_rightEncoder);
    addChild("Rangefinder", m_rangefinder);
    addChild("Gyro", m_gyro);
  }

  /**
   * The log method puts interesting information to the SmartDashboard.
   */
  public void log() {
    SmartDashboard.putNumber("Left Distance", m_leftEncoder.getDistance());
    SmartDashboard.putNumber("Right Distance", m_rightEncoder.getDistance());
    SmartDashboard.putNumber("Left Speed", m_leftEncoder.getRate());
    SmartDashboard.putNumber("Right Speed", m_rightEncoder.getRate());
    SmartDashboard.putNumber("Gyro", m_gyro.getAngle());
  }

  /**
   * Tank style driving for the DriveTrain.
   *
   * @param left  Speed in range [-1,1]
   * @param right Speed in range [-1,1]
   */
  public void drive(double left, double right) {
    m_drive.tankDrive(left, right);
  }

  /**
   * Get the robot's heading.
   *
   * @return The robots heading in degrees from 0 to 360.
   */
  public double getHeading() {
    // Negating the angle because WPILib gyros are CW positive.
    return -m_gyro.getAngle();
  }

  /**
   * Reset the robots sensors to the zero states.
   */
  public void reset() {
    m_gyro.reset();
    m_leftEncoder.reset();
    m_rightEncoder.reset();
  }

  /**
   * Get the average distance of the encoders since the last reset.
   *
   * @return The distance driven (average of left and right encoders).
   */
  public double getDistance() {
    return (m_leftEncoder.getDistance() + m_rightEncoder.getDistance()) / 2;
  }

  /**
   * Get the distance to the obstacle.
   *
   * @return The distance to the obstacle detected by the rangefinder.
   */
  public double getDistanceToObstacle() {
    // Really meters in simulation since it's a rangefinder...
    return m_rangefinder.getAverageVoltage();
  }
}
