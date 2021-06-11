// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.math.geometry.Rotation2d;
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
   * Set the position using an {@link Rotation2d}.
   *
   * @param angle The angle.
   */
  public void setPosition(Rotation2d angle) {
    setTurns(angle.getDegrees() / 360.0);
  }

  /**
   * Set the position of the encoder.
   *
   * @param turns The position.
   */
  public void setTurns(double turns) {
    m_simPosition.set(turns);
  }

  /**
   * Get the simulated position.
   *
   * @return The simulated position.
   */
  public double getTurns() {
    return m_simPosition.get();
  }

  /**
   * Get the position as a {@link Rotation2d}.
   *
   * @return The position as a {@link Rotation2d}.
   */
  public Rotation2d getPosition() {
    return Rotation2d.fromDegrees(getTurns() * 360.0);
  }
}
