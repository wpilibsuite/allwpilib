package edu.wpi.first.wpilibj.examples.periodvariantflywheel.subsystems;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Notifier;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.Timer;

public class Flywheel {
  private final Spark m_motor = new Spark(1);
  private final Encoder m_encoder = new Encoder(1, 2);

  private final FlywheelController m_wheel = new FlywheelController();
  private final Notifier m_thread = new Notifier(this::iterate);
  private double m_lastTime = -1.0;

  public Flywheel() {
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

  /**
   * Iterates the shooter control loop one cycle.
   */
  public void iterate() {
    m_wheel.setMeasuredVelocity(m_encoder.getRate());

    double now = Timer.getFPGATimestamp();
    if (m_lastTime != -1.0) {
      m_wheel.update(now - m_lastTime);
    } else {
      m_wheel.update(0.005);
    }

    double batteryVoltage = RobotController.getBatteryVoltage();
    m_motor.set(m_wheel.getControllerVoltage() / batteryVoltage);

    m_lastTime = now;
  }

  public void reset() {
    m_wheel.reset();
    m_lastTime = -1.0;
  }
}
