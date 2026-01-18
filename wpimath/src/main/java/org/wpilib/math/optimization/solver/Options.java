// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization.solver;

/** Solver options. */
public class Options {
  /** The solver will stop once the error is below this tolerance. */
  public double tolerance = 1e-8;

  /** The maximum number of solver iterations before returning a solution. */
  public int maxIterations = 5000;

  /** The maximum elapsed wall clock time in seconds before returning a solution. */
  public double timeout = Double.POSITIVE_INFINITY;

  /**
   * Enables the feasible interior-point method. When the inequality constraints are all feasible,
   * step sizes are reduced when necessary to prevent them becoming infeasible again. This is useful
   * when parts of the problem are ill-conditioned in infeasible regions (e.g., square root of a
   * negative value). This can slow or prevent progress toward a solution though, so only enable it
   * if necessary.
   */
  public boolean feasibleIPM = false;

  /**
   * Enables diagnostic prints.
   *
   * <table>
   *   <tr>
   *     <th>Heading</th>
   *     <th>Description</th>
   *   </tr>
   *   <tr>
   *     <td>iter</td>
   *     <td>Iteration number</td>
   *   </tr>
   *   <tr>
   *     <td>type</td>
   *     <td>Iteration type (normal, accepted second-order correction, rejected
   *     second-order correction)</td>
   *   </tr>
   *   <tr>
   *     <td>time (ms)</td>
   *     <td>Duration of iteration in milliseconds</td>
   *   </tr>
   *   <tr>
   *     <td>error</td>
   *     <td>Error estimate</td>
   *   </tr>
   *   <tr>
   *     <td>cost</td>
   *     <td>Cost function value at current iterate</td>
   *   </tr>
   *   <tr>
   *     <td>infeas.</td>
   *     <td>Constraint infeasibility at current iterate</td>
   *   </tr>
   *   <tr>
   *     <td>complement.</td>
   *     <td>Complementary slackness at current iterate (sᵀz)</td>
   *   </tr>
   *   <tr>
   *     <td>μ</td>
   *     <td>Barrier parameter</td>
   *   </tr>
   *   <tr>
   *     <td>reg</td>
   *     <td>Iteration matrix regularization</td>
   *   </tr>
   *   <tr>
   *     <td>primal α</td>
   *     <td>Primal step size</td>
   *   </tr>
   *   <tr>
   *     <td>dual α</td>
   *     <td>Dual step size</td>
   *   </tr>
   *   <tr>
   *     <td>↩</td>
   *     <td>Number of line search backtracks</td>
   *   </tr>
   * </table>
   */
  public boolean diagnostics = false;

  /** Default options. */
  public Options() {}

  /**
   * Set tolerance.
   *
   * @param tolerance The solver will stop once the error is below this tolerance.
   * @return This Options object.
   */
  public Options withTolerance(double tolerance) {
    this.tolerance = tolerance;
    return this;
  }

  /**
   * Set max iterations.
   *
   * @param maxIterations The maximum number of solver iterations before returning a solution.
   * @return This Options object.
   */
  public Options withMaxIterations(int maxIterations) {
    this.maxIterations = maxIterations;
    return this;
  }

  /**
   * Set timeout.
   *
   * @param timeout The maximum elapsed wall clock time in seconds before returning a solution.
   * @return This Options object.
   */
  public Options withTimeout(double timeout) {
    this.timeout = timeout;
    return this;
  }

  /**
   * Enable or disable feasible IPM.
   *
   * @param feasibleIPM Enables the feasible interior-point method. When the inequality constraints
   *     are all feasible, step sizes are reduced when necessary to prevent them becoming infeasible
   *     again. This is useful when parts of the problem are ill-conditioned in infeasible regions
   *     (e.g., square root of a negative value). This can slow or prevent progress toward a
   *     solution though, so only enable it if necessary.
   * @return This Options object.
   */
  public Options withFeasibleIPM(boolean feasibleIPM) {
    this.feasibleIPM = feasibleIPM;
    return this;
  }

  /**
   * Enable or disable diagnostics.
   *
   * @param diagnostics Enables diagnostic prints.
   * @return This Options object.
   */
  public Options withDiagnostics(boolean diagnostics) {
    this.diagnostics = diagnostics;
    return this;
  }
}
