/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.AnalogEncoder;
import edu.wpi.first.wpilibj.geometry.Rotation2d;

/**
 * Class to control a simulated analog encoder.
 */
public class AnalogEncoderSim {
  private final SimDeviceSim m_wrappedSimDevice;
  private final SimDouble m_simPosition;

  /**
   * Constructs from an AnalogEncoder object.
   *
   * @param encoder AnalogEncoder to simulate
   */
  public AnalogEncoderSim(AnalogEncoder encoder) {
    m_wrappedSimDevice = new SimDeviceSim("AnalogEncoder" + "[" + encoder.getChannel() + "]");
    m_simPosition = m_wrappedSimDevice.getDouble("Position");
  }

  public void setDisplayName(String displayName) {
    m_wrappedSimDevice.setDisplayName(displayName);
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
   */
  public double getTurns() {
    return m_simPosition.get();
  }

  /**
   * Get the position as a {@link Rotation2d}.
   */
  public Rotation2d getPosition() {
    return Rotation2d.fromDegrees(getTurns() * 360.0);
  }
}
