// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.pidwrappers;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.PIDSource;
import edu.wpi.first.wpilibj.PIDSourceType;
import edu.wpi.first.wpilibj.Ultrasonic;

/**
 * Wrapper so that PIDSource is implemented for Ultrasonic for old PIDController.
 *
 * @deprecated Use {@link edu.wpi.first.math.controller.PIDController} which doesn't require this
 *     wrapper.
 */
@Deprecated(since = "2022", forRemoval = true)
public class PIDUltrasonic extends Ultrasonic implements PIDSource {
  protected PIDSourceType m_pidSource = PIDSourceType.kDisplacement;

  public PIDUltrasonic(int pingChannel, int echoChannel) {
    super(pingChannel, echoChannel);
  }

  public PIDUltrasonic(DigitalOutput pingChannel, DigitalInput echoChannel) {
    super(pingChannel, echoChannel);
  }

  @Override
  public void setPIDSourceType(PIDSourceType pidSource) {
    if (!pidSource.equals(PIDSourceType.kDisplacement)) {
      throw new IllegalArgumentException("Only displacement PID is allowed for ultrasonics.");
    }
    m_pidSource = pidSource;
  }

  @Override
  public PIDSourceType getPIDSourceType() {
    return m_pidSource;
  }

  /**
   * Get the range in the inches for the PIDSource base object.
   *
   * @return The range in inches
   */
  @Override
  public double pidGet() {
    return getRangeInches();
  }
}
