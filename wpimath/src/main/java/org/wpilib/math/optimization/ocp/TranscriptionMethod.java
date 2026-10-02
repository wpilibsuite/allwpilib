// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization.ocp;

/** Enum describing an OCP transcription method. */
public enum TranscriptionMethod {
  /**
   * Each state is a decision variable constrained to the integrated dynamics of the previous state.
   */
  DIRECT_TRANSCRIPTION(0),
  /**
   * The trajectory is modeled as a series of cubic polynomials where the centerpoint slope is
   * constrained.
   */
  DIRECT_COLLOCATION(1),
  /** States depend explicitly as a function of all previous states and all previous inputs. */
  SINGLE_SHOOTING(2);

  /** TranscriptionMethod value. */
  public final int value;

  TranscriptionMethod(int value) {
    this.value = value;
  }
}
