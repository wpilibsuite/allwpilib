// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.DutyCycleEncoder;

/** Class to control a simulated duty cycle encoder. */
public class DutyCycleEncoderSim {
  private final SimDouble m_simPosition;
  private final SimBoolean m_simIsConnected;

  /**
   * Constructs from an DutyCycleEncoder object.
   *
   * @param encoder DutyCycleEncoder to simulate
   */
  public DutyCycleEncoderSim(DutyCycleEncoder encoder) {
    this(encoder.getSourceChannel());
  }

  /**
   * Constructs from a digital input channel.
   *
   * @param channel digital input channel.
   */
  public DutyCycleEncoderSim(int channel) {
    SimDeviceSim wrappedSimDevice = new SimDeviceSim("DutyCycle:DutyCycleEncoder", channel);
    m_simPosition = wrappedSimDevice.getDouble("Position");
    m_simIsConnected = wrappedSimDevice.getBoolean("Connected");
  }

  /**
   * Get the position in turns.
   *
   * @return The position.
   */
  public double get() {
    return m_simPosition.get();
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
   * Get if the encoder is connected.
   *
   * @return true if the encoder is connected.
   */
  public boolean getConnected() {
    return m_simIsConnected.get();
  }

  /**
   * Set if the encoder is connected.
   *
   * @param isConnected Whether or not the sensor is connected.
   */
  public void setConnected(boolean isConnected) {
    m_simIsConnected.set(isConnected);
  }
}
