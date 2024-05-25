// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim;

import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.StorageConstants;
import edu.wpi.first.wpilibj.simulation.DIOSim;
import edu.wpi.first.wpilibj.simulation.PWMSim;

/** Simulation controller for the storage subsystem. */
public class StorageSim {
  private final PWMSim m_motor = new PWMSim(StorageConstants.kMotorPort);
  private final DIOSim m_ballSensor = new DIOSim(StorageConstants.kBallSensorPort);

  /** Call this to advance the simulation by 20 ms. */
  public void simulationPeriodic() {}

  /**
   * Get the duty cycle commanded to the storage motor.
   *
   * @return currently-commanded duty cycle
   */
  public double getMotor() {
    return m_motor.getSpeed();
  }

  /**
   * Set whether the storage is currently full.
   *
   * @param full true for full.
   */
  public void setIsFull(boolean full) {
    m_ballSensor.setValue(full);
  }

  /** Reset all simulation state. */
  public void reset() {
    m_motor.resetData();
    m_ballSensor.resetData();
  }
}
