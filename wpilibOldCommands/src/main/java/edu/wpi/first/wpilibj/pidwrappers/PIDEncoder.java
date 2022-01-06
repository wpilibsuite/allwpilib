// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.pidwrappers;

import edu.wpi.first.wpilibj.DigitalSource;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PIDSource;
import edu.wpi.first.wpilibj.PIDSourceType;

/**
 * Wrapper so that PIDSource is implemented for Encoder for old PIDController.
 *
 * @deprecated Use {@link edu.wpi.first.math.controller.PIDController} which doesn't require this
 *     wrapper.
 */
@Deprecated(since = "2022", forRemoval = true)
public class PIDEncoder extends Encoder implements PIDSource {
  private PIDSourceType m_pidSource = PIDSourceType.kDisplacement;

  public PIDEncoder(final int channelA, final int channelB, boolean reverseDirection) {
    super(channelA, channelB, reverseDirection, EncodingType.k4X);
  }

  public PIDEncoder(final int channelA, final int channelB) {
    super(channelA, channelB, false);
  }

  public PIDEncoder(
      final int channelA,
      final int channelB,
      boolean reverseDirection,
      final EncodingType encodingType) {
    super(channelA, channelB, reverseDirection, encodingType);
  }

  public PIDEncoder(
      final int channelA, final int channelB, final int indexChannel, boolean reverseDirection) {
    super(channelA, channelB, indexChannel, reverseDirection);
  }

  public PIDEncoder(final int channelA, final int channelB, final int indexChannel) {
    super(channelA, channelB, indexChannel, false);
  }

  public PIDEncoder(DigitalSource sourceA, DigitalSource sourceB, boolean reverseDirection) {
    super(sourceA, sourceB, reverseDirection, EncodingType.k4X);
  }

  public PIDEncoder(DigitalSource sourceA, DigitalSource sourceB) {
    super(sourceA, sourceB, false);
  }

  public PIDEncoder(
      DigitalSource sourceA,
      DigitalSource sourceB,
      boolean reverseDirection,
      final EncodingType encodingType) {
    super(sourceA, sourceB, reverseDirection, encodingType);
  }

  public PIDEncoder(
      DigitalSource sourceA,
      DigitalSource sourceB,
      DigitalSource indexSource,
      boolean reverseDirection) {
    super(sourceA, sourceB, indexSource, reverseDirection);
  }

  public PIDEncoder(DigitalSource sourceA, DigitalSource sourceB, DigitalSource indexSource) {
    super(sourceA, sourceB, indexSource, false);
  }

  /**
   * Set which parameter of the encoder you are using as a process control variable. The encoder
   * class supports the rate and distance parameters.
   *
   * @param pidSource An enum to select the parameter.
   */
  @Override
  public void setPIDSourceType(PIDSourceType pidSource) {
    m_pidSource = pidSource;
  }

  @Override
  public PIDSourceType getPIDSourceType() {
    return m_pidSource;
  }

  /**
   * Implement the PIDSource interface.
   *
   * @return The current value of the selected source parameter.
   */
  @Override
  public double pidGet() {
    switch (m_pidSource) {
      case kDisplacement:
        return getDistance();
      case kRate:
        return getRate();
      default:
        return 0.0;
    }
  }
}
