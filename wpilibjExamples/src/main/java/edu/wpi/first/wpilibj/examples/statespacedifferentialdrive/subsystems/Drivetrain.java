/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.statespacedifferentialdrive.subsystems;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Notifier;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.SpeedControllerGroup;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;

@SuppressWarnings("PMD.UnusedPrivateField")
public class Drivetrain {
  private final Spark m_leftFront = new Spark(1);
  private final Spark m_leftRear = new Spark(2);
  private final SpeedControllerGroup m_left = new SpeedControllerGroup(m_leftFront, m_leftRear);
  private final Encoder m_leftEncoder = new Encoder(1, 2);

  private final Spark m_rightFront = new Spark(3);
  private final Spark m_rightRear = new Spark(4);
  private final SpeedControllerGroup m_right = new SpeedControllerGroup(m_rightFront, m_rightRear);
  private final Encoder m_rightEncoder = new Encoder(3, 4);

  private final DifferentialDrive m_drive = new DifferentialDrive(m_left, m_right);

  private final DifferentialDriveController m_drivetrain = new DifferentialDriveController();
  private final Notifier m_thread = new Notifier(this::iterate);

  public Drivetrain() {
    m_leftEncoder.setDistancePerPulse(2.0 * Math.PI / 360.0);
    m_rightEncoder.setDistancePerPulse(2.0 * Math.PI / 360.0);
  }

  public void enable() {
    m_drivetrain.enable();
    m_thread.startPeriodic(0.005);
  }

  public void disable() {
    m_drivetrain.disable();
    m_thread.stop();
  }

  /**
   * Sets the references.
   *
   * @param leftPosition  Position of left side in meters.
   * @param leftVelocity  Velocity of left side in meters per second.
   * @param rightPosition Position of right side in meters.
   * @param rightVelocity Velocity of right side in meters per second.
   */
  public void setReferences(double leftPosition, double leftVelocity,
                            double rightPosition, double rightVelocity) {
    m_drivetrain.setReferences(leftPosition, leftVelocity,
        rightPosition, rightVelocity);
  }

  public boolean atReferences() {
    return m_drivetrain.atReferences();
  }

  /**
   * Iterates the drivetrain control loop one cycle.
   */
  public void iterate() {
    m_drivetrain.setMeasuredStates(m_leftEncoder.getDistance(),
        m_rightEncoder.getDistance());

    m_drivetrain.update();

    double batteryVoltage = RobotController.getBatteryVoltage();
    m_left.set(m_drivetrain.getControllerLeftVoltage() / batteryVoltage);
    m_right.set(m_drivetrain.getControllerRightVoltage() / batteryVoltage);
  }

  public double getControllerLeftVoltage() {
    return m_drivetrain.getControllerLeftVoltage();
  }

  public double getControllerRightVoltage() {
    return m_drivetrain.getControllerRightVoltage();
  }

  public void reset() {
    m_drivetrain.reset();
  }
}
