/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.periodvariantelevator.subsystems;


import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Notifier;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.Timer;

public class Elevator {
  private final Spark m_motor = new Spark(1);
  private final Encoder m_encoder = new Encoder(1, 2);

  private final ElevatorController m_elevator = new ElevatorController();
  private final Notifier m_thread = new Notifier(this::iterate);
  private double m_lastTime = -1.0;

  public Elevator() {
    m_encoder.setDistancePerPulse(2.0 * Math.PI / 360.0);
  }

  public void enable() {
    m_elevator.enable();
    m_thread.startPeriodic(0.005);
  }

  public void disable() {
    m_elevator.disable();
    m_thread.stop();
  }

  /**
   * Sets the references.
   *
   * @param position Position of the carriage in meters.
   * @param velocity Velocity of the carriage in meters per second.
   */
  public void setReferences(double position, double velocity) {
    m_elevator.setReferences(position, velocity);
  }

  public boolean atReferences() {
    return m_elevator.atReferences();
  }

  /**
   * Iterates the elevator control loop one cycle.
   */
  public void iterate() {
    m_elevator.setMeasuredPosition(m_encoder.getDistance());

    double now = Timer.getFPGATimestamp();
    if (m_lastTime != -1.0) {
      m_elevator.update(now - m_lastTime);
    } else {
      m_elevator.update(0.005);
    }

    double batteryVoltage = RobotController.getBatteryVoltage();
    m_motor.set(m_elevator.getControllerVoltage() / batteryVoltage);

    m_lastTime = now;
  }

  public double getControllerVoltage() {
    return m_elevator.getControllerVoltage();
  }

  public void reset() {
    m_elevator.reset();
    m_lastTime = -1.0;
  }
}
