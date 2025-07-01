// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization.ocp;

/** Enum describing the type of system timestep. */
public enum TimestepMethod {
  /** The timestep is a fixed constant. */
  FIXED(0),
  /** The timesteps are allowed to vary as independent decision variables. */
  VARIABLE(1),
  /** The timesteps are equal length but allowed to vary as a single decision variable. */
  VARIABLE_SINGLE(2);

  /** TimestepMethod value. */
  public final int value;

  TimestepMethod(int value) {
    this.value = value;
  }
}
