// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import org.wpilib.math.autodiff.Variable;

/** A vector of equality constraints of the form cₑ(x) = 0. */
@SuppressWarnings("PMD.ArrayIsStoredDirectly")
public class EqualityConstraints {
  /** List of equality constraints. */
  public Variable[] constraints;

  /**
   * Constructs an EqualityConstraints.
   *
   * @param constraints The constraints.
   */
  public EqualityConstraints(Variable[] constraints) {
    this.constraints = constraints;
  }
}
