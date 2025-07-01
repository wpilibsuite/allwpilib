// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization.ocp;

import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableMatrix;

/**
 * Function representing an explicit or implicit ODE, or a discrete state transition function.
 *
 * <ul>
 *   <li>Explicit: dx/dt = f(t, x, u, *)
 *   <li>Implicit: f(t, [x dx/dt]', u, *) = 0
 *   <li>State transition: xₖ₊₁ = f(t, xₖ, uₖ, dt)
 * </ul>
 */
@FunctionalInterface
public interface DynamicsFunction {
  /**
   * Applies this function with the arguments and returns the result.
   *
   * @param t Time in seconds.
   * @param x State vector.
   * @param u Input vector.
   * @param dt Timestep duration in seconds.
   * @return The state derivative dx/dt.
   */
  VariableMatrix apply(Variable t, VariableMatrix x, VariableMatrix u, Variable dt);
}
