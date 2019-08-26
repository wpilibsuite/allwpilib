package edu.wpi.first.wpilibj.examples.statespacesinglejointedarm.subsystems;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Notifier;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.Spark;

public class SingleJointedArm {
  private final Spark m_motor = new Spark(1);
  private final Encoder m_encoder = new Encoder(1, 2);

  private final SingleJointedArmController m_singleJointedArm = new SingleJointedArmController();
  private final Notifier m_thread = new Notifier(this::iterate);

  public SingleJointedArm() {
    m_encoder.setDistancePerPulse(2.0 * Math.PI / 360.0);
  }

  public void enable() {
    m_singleJointedArm.enable();
    m_thread.startPeriodic(0.005);
  }

  public void disable() {
    m_singleJointedArm.disable();
    m_thread.stop();
  }

  /**
   * Sets the references.
   *
   * @param angle           Angle of arm in radians.
   * @param angularVelocity Velocity of arm in radians per second.
   */
  public void setReferences(double angle, double angularVelocity) {
    m_singleJointedArm.setReferences(angle, angularVelocity);
  }

  public boolean atReferences() {
    return m_singleJointedArm.atReferences();
  }

  /**
   * Iterates the elevator control loop one cycle.
   */
  public void iterate() {
    m_singleJointedArm.setMeasuredAngle(m_encoder.getDistance());
    m_singleJointedArm.update();

    double batteryVoltage = RobotController.getBatteryVoltage();
    m_motor.set(m_singleJointedArm.getControllerVoltage() / batteryVoltage);
  }

  public double getControllerVoltage() {
    return m_singleJointedArm.getControllerVoltage();
  }

  public void reset() {
    m_singleJointedArm.reset();
  }


}
