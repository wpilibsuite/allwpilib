package edu.wpi.first.wpilibj.examples.periodvariantdifferentialdrive.subsystems;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Notifier;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.SpeedControllerGroup;
import edu.wpi.first.wpilibj.Timer;
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
  private final Encoder m_rightEncoder = new Encoder(1, 2);

  private final DifferentialDrive m_drive = new DifferentialDrive(m_left, m_right);

  private final DifferentialDriveController m_drivetrain = new DifferentialDriveController();
  private final Notifier m_thread = new Notifier(this::iterate);
  private double m_lastTime = -1.0;

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

  public void setReferences(double leftPosition, double leftVelocity,
                            double rightPosition, double rightVelocity) {
    m_drivetrain.setReferences(leftPosition, leftVelocity, rightPosition, rightVelocity);
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

    double now = Timer.getFPGATimestamp();
    if (m_lastTime != -1.0) {
      m_drivetrain.update(now - m_lastTime);
    } else {
      m_drivetrain.update(0.005);
    }

    double batteryVoltage = RobotController.getBatteryVoltage();
    m_left.set(m_drivetrain.getControllerLeftVoltage() / batteryVoltage);
    m_right.set(m_drivetrain.getControllerRightVoltage() / batteryVoltage);

    m_lastTime = now;
  }

  public double getControllerLeftVoltage() {
    return m_drivetrain.getControllerLeftVoltage();
  }

  public double getControllerRightVoltage() {
    return m_drivetrain.getControllerRightVoltage();
  }

  public void reset() {
    m_drivetrain.reset();
    m_lastTime = -1.0;
  }
}
