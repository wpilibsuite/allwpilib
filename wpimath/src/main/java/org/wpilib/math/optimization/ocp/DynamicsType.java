// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization.ocp;

/** Enum describing a type of system dynamics constraints. */
public enum DynamicsType {
  /** The dynamics are a function in the form dx/dt = f(t, x, u). */
  EXPLICIT_ODE(0),
  /** The dynamics are a function in the form xₖ₊₁ = f(t, xₖ, uₖ). */
  DISCRETE(1);

  /** DynamicsType value. */
  public final int value;

  DynamicsType(int value) {
    this.value = value;
  }
}
