package edu.wpi.first.wpilibj.examples.statespaceflywheel.subsystems;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Notifier;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.Spark;

public class Flywheel {
  private Spark m_motor = new Spark(1);
  private Encoder m_encoder = new Encoder(1, 2);

  private FlywheelController m_wheel;
  private Notifier m_thread;

  public Flywheel() {
    m_wheel = new FlywheelController();
    m_thread = new Notifier(this::iterate);
    m_encoder.setDistancePerPulse(2.0 * Math.PI / 360.0);
  }

  public void enable() {
    m_wheel.enable();
    m_thread.startPeriodic(0.005);
  }

  public void disable() {
    m_wheel.disable();
    m_thread.stop();
  }

  public void setReference(double angularVelocity) {
    m_wheel.setVelocityReference(angularVelocity);
  }

  public boolean atReference() {
    return m_wheel.atReference();
  }

  public void iterate() {
    m_wheel.setMeasuredVelocity(m_encoder.getRate());
    m_wheel.update();

    double batteryVoltage = RobotController.getBatteryVoltage();
    m_motor.set(m_wheel.getControllerVoltage() / batteryVoltage);
  }

  public void reset() {
    m_wheel.reset();
  }
}
