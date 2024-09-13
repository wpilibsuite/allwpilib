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
  private final SimDouble m_simDistancePerRotation;
  private final SimDouble m_simAbsolutePosition;
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
    m_simPosition = wrappedSimDevice.getDouble("position");
    m_simDistancePerRotation = wrappedSimDevice.getDouble("distance_per_rot");
    m_simAbsolutePosition = wrappedSimDevice.getDouble("absPosition");
    m_simIsConnected = wrappedSimDevice.getBoolean("connected");
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
   * Set the position in turns.
   *
   * @param turns The position.
   */
  public void set(double turns) {
    m_simPosition.set(turns);
  }

  /**
   * Get the distance.
   *
   * @return The distance.
   */
  public double getDistance() {
    return m_simPosition.get() * m_simDistancePerRotation.get();
  }

  /**
   * Set the distance.
   *
   * @param distance The distance.
   */
  public void setDistance(double distance) {
    m_simPosition.set(distance / m_simDistancePerRotation.get());
  }

  /**
   * Get the absolute position.
   *
   * @return The absolute position
   */
  public double getAbsolutePosition() {
    return m_simAbsolutePosition.get();
  }

  /**
   * Set the absolute position.
   *
   * @param position The absolute position
   */
  public void setAbsolutePosition(double position) {
    m_simAbsolutePosition.set(position);
  }

  /**
   * Get the distance per rotation for this encoder.
   *
   * @return The scale factor that will be used to convert rotation to useful units.
   */
  public double getDistancePerRotation() {
    return m_simDistancePerRotation.get();
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
