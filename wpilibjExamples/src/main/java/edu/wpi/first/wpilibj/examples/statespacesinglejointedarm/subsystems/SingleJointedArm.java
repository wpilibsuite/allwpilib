package edu.wpi.first.wpilibj.examples.statespacesinglejointedarm.subsystems;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Notifier;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.Spark;

public class SingleJointedArm {
  private Spark m_motor = new Spark(1);
  private Encoder m_encoder = new Encoder(1, 2);

  private SingleJointedArmController m_singleJointedArm;
  private Notifier m_thread;

  public SingleJointedArm() {
    m_singleJointedArm = new SingleJointedArmController();
    m_thread = new Notifier(this::iterate);

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

  public void setReferences(double angle, double angularVelocity) {
    m_singleJointedArm.setReferences(angle, angularVelocity);
  }

  public boolean atReferences() {
    return m_singleJointedArm.atReferences();
  }

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
