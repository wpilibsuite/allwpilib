// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization.ocp;

import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableMatrix;

/**
 * A callback f(t, x, u, dt) where t is time, x is the state vector, u is the input vector, and dt
 * is the timestep duration.
 */
@FunctionalInterface
public interface ConstraintEvaluationFunction {
  /**
   * Applies this function with the arguments.
   *
   * @param t Time in seconds.
   * @param x State vector.
   * @param u Input vector.
   * @param dt Timestep duration in seconds.
   */
  void accept(Variable t, VariableMatrix x, VariableMatrix u, Variable dt);
}
