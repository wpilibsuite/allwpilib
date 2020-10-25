/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.DutyCycleEncoder;

/**
 * Class to control a simulated duty cycle encoder.
 */
public class DutyCycleEncoderSim {
  private final SimDouble m_simPosition;
  private final SimDouble m_simDistancePerRotation;

  /**
   * Constructs from an DutyCycleEncoder object.
   *
   * @param encoder DutyCycleEncoder to simulate
   */
  public DutyCycleEncoderSim(DutyCycleEncoder encoder) {
    SimDeviceSim wrappedSimDevice = new SimDeviceSim("DutyCycleEncoder" + "[" + encoder.getFPGAIndex() + "]");
    m_simPosition = wrappedSimDevice.getDouble("Position");
    m_simDistancePerRotation = wrappedSimDevice.getDouble("DistancePerRotation");
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
   */
  public void setDistance(double distance) {
    m_simPosition.set(distance / m_simDistancePerRotation.get());
  }
}
