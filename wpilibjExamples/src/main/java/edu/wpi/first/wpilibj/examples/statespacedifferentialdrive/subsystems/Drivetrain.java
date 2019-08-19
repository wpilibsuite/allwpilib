package edu.wpi.first.wpilibj.examples.statespacedifferentialdrive.subsystems;

import edu.wpi.first.wpilibj.*;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;

public class Drivetrain {
  private Spark m_leftFront = new Spark(1);
  private Spark m_leftRear = new Spark(2);
  private SpeedControllerGroup m_left = new SpeedControllerGroup(m_leftFront, m_leftRear);
  private Encoder m_leftEncoder = new Encoder(1, 2);

  private Spark m_rightFront = new Spark(3);
  private Spark m_rightRear = new Spark(4);
  private SpeedControllerGroup m_right = new SpeedControllerGroup(m_rightFront, m_rightRear);
  private Encoder m_rightEncoder = new Encoder(3, 4);

  private DifferentialDrive m_drive = new DifferentialDrive(m_left, m_right);

  private DifferentialDriveController m_drivetrain;
  private Notifier m_thread;

  public Drivetrain() {
    m_drivetrain = new DifferentialDriveController();
    m_thread = new Notifier(this::iterate);
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

  public void setReferences(double leftPosition, double leftVelocity,
                            double rightPosition, double rightVelocity) {
    m_drivetrain.setReferences(leftPosition, leftVelocity,
        rightPosition, rightVelocity);
  }

  public boolean atReferences() {
    return m_drivetrain.atReferences();
  }

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
