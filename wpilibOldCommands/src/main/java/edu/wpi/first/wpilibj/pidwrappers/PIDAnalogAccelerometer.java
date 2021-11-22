// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.pidwrappers;

import edu.wpi.first.wpilibj.AnalogAccelerometer;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.PIDSource;
import edu.wpi.first.wpilibj.PIDSourceType;

/**
 * Wrapper so that PIDSource is implemented for AnalogAccelerometer for old PIDController
 */
public class PIDAnalogAccelerometer extends AnalogAccelerometer implements PIDSource {
  protected PIDSourceType m_pidSource = PIDSourceType.kDisplacement;

  public PIDAnalogAccelerometer(int channel) {
    super(channel);
  }

  public PIDAnalogAccelerometer(AnalogInput channel) {
    super(channel);
  }

  @Override
  public void setPIDSourceType(PIDSourceType pidSource) {
    m_pidSource = pidSource;
  }

  @Override
  public PIDSourceType getPIDSourceType() {
    return m_pidSource;
  }

  /**
   * Get the Acceleration for the PID Source parent.
   *
   * @return The current acceleration in Gs.
   */
  @Override
  public double pidGet() {
    return getAcceleration();
  }
}
