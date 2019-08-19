package edu.wpi.first.wpilibj.examples.statespaceelevator.subsystems;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Notifier;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.Spark;

public class Elevator {
  private Spark m_motor = new Spark(1);
  private Encoder m_encoder = new Encoder(1, 2);
  private ElevatorController m_elevator;
  private Notifier m_thread;

  /**
   * Constructs a new Elevator, and prepares the notifier and controller to run.
   */
  public Elevator() {
    m_elevator = new ElevatorController();
    m_encoder.setDistancePerPulse(2.0 * Math.PI / 360.0);
    m_thread = new Notifier(this::iterate);
  }

  public void enable() {
    m_elevator.enable();
    m_thread.startPeriodic(0.005);
  }

  public void disable() {
    m_elevator.disable();
    m_thread.stop();
  }

  public void setReferences(double position, double velocity) {
    m_elevator.setReferences(position, velocity);
  }

  /**
   * Returns whether the controller is at its references.
   *
   * @return Whether the controller is at its references
   */
  public boolean atReference() {
    return m_elevator.atReferences();
  }

  /**
   * Updates the state space loop for the elevator,
   * and writes the new update to the spark.
   */
  public void iterate() {
    m_elevator.setMeasuredPosition(m_encoder.getDistance());
    m_elevator.update();

    double batteryVoltage = RobotController.getBatteryVoltage();
    m_motor.set(m_elevator.getControllerVoltage() / batteryVoltage);
  }

  public double getControllerVoltage() {
    return m_elevator.getControllerVoltage();
  }

  public void reset() {
    m_elevator.reset();
  }
}
