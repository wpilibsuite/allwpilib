// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.AnalogEncoder;

/** Class to control a simulated analog encoder. */
public class AnalogEncoderSim {
  private final SimDouble m_simPosition;

  /**
   * Constructs from an AnalogEncoder object.
   *
   * @param encoder AnalogEncoder to simulate
   */
  public AnalogEncoderSim(AnalogEncoder encoder) {
    SimDeviceSim wrappedSimDevice =
        new SimDeviceSim("AnalogEncoder" + "[" + encoder.getChannel() + "]");
    m_simPosition = wrappedSimDevice.getDouble("Position");
  }

  /**
   * Set the position.
   *
   * @param value The position.
   */
  public void set(double value) {
    m_simPosition.set(value);
  }

  /**
   * Get the simulated position.
   *
   * @return The simulated position.
   */
  public double get() {
    return m_simPosition.get();
  }
}
