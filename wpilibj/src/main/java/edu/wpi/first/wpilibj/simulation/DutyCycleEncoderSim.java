// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.DutyCycleEncoder;

/** Class to control a simulated duty cycle encoder. */
public class DutyCycleEncoderSim {
  private final SimDouble m_simPosition;
  private final SimDouble m_simDistancePerRotation;

  /**
   * Constructs from an DutyCycleEncoder object.
   *
   * @param encoder DutyCycleEncoder to simulate
   */
  public DutyCycleEncoderSim(DutyCycleEncoder encoder) {
    SimDeviceSim wrappedSimDevice =
        new SimDeviceSim("DutyCycle:DutyCycleEncoder" + "[" + encoder.getSourceChannel() + "]");
    m_simPosition = wrappedSimDevice.getDouble("position");
    m_simDistancePerRotation = wrappedSimDevice.getDouble("distance_per_rot");
  }

  /**
   * Set the position in turns.
   *
   * @param turns The position.
   */
  public void set(double turns) {
    m_simPosition.set(turns);
  }

  /**
   * Set the position.
   *
   * @param distance The position.
   */
  public void setDistance(double distance) {
    m_simPosition.set(distance / m_simDistancePerRotation.get());
  }
}
