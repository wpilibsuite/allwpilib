// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import org.wpilib.math.autodiff.Variable;

/** A vector of inequality constraints of the form cᵢ(x) ≥ 0. */
@SuppressWarnings("PMD.ArrayIsStoredDirectly")
public class InequalityConstraints {
  /** List of inequality constraints. */
  public Variable[] constraints;

  /**
   * Constructs an InequalityConstraints.
   *
   * @param constraints The constraints.
   */
  public InequalityConstraints(Variable[] constraints) {
    this.constraints = constraints;
  }
}
