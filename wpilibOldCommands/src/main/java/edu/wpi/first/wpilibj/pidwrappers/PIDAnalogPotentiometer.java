// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.pidwrappers;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.PIDSource;
import edu.wpi.first.wpilibj.PIDSourceType;

/**
 * Wrapper so that PIDSource is implemented for AnalogPotentiometer for old PIDController.
 */
public class PIDAnalogPotentiometer extends AnalogPotentiometer implements PIDSource {
  protected PIDSourceType m_pidSource = PIDSourceType.kDisplacement;

  public PIDAnalogPotentiometer(int channel, double fullRange, double offset) {
    super(channel, fullRange, offset);
  }

  public PIDAnalogPotentiometer(AnalogInput input, double fullRange, double offset) {
    super(input, fullRange, offset);
  }

  public PIDAnalogPotentiometer(int channel, double scale) {
    super(channel, scale);
  }

  public PIDAnalogPotentiometer(AnalogInput input, double scale) {
    super(input, scale);
  }

  public PIDAnalogPotentiometer(int channel) {
    super(channel);
  }

  public PIDAnalogPotentiometer(AnalogInput input) {
    super(input);
  }

  @Override
  public void setPIDSourceType(PIDSourceType pidSource) {
    if (!pidSource.equals(PIDSourceType.kDisplacement)) {
      throw new IllegalArgumentException("Only displacement PID is allowed for potentiometers.");
    }
    m_pidSource = pidSource;
  }

  @Override
  public PIDSourceType getPIDSourceType() {
    return m_pidSource;
  }

  /**
   * Implement the PIDSource interface.
   *
   * @return The current reading.
   */
  @Override
  public double pidGet() {
    return get();
  }
}
