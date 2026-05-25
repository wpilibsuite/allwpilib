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
   * Enables the feasible interior-point method.
   *
   * <p>When the inequality constraints are all feasible, step sizes are reduced when necessary to
   * prevent them becoming infeasible again. This is useful when parts of the problem are
   * ill-conditioned in infeasible regions (e.g., square root of a negative value). This can slow or
   * prevent progress toward a solution though, so only enable it if necessary.
   */
  public boolean feasibleIPM = false;

  /**
   * Enables diagnostic output.
   *
   * <p>See <a
   * href="https://sleipnirgroup.github.io/Sleipnir/md_usage.html#output">https://sleipnirgroup.github.io/Sleipnir/md_usage.html#output</a>
   * for more information.
   */
  public boolean diagnostics = false;

  /** Default options. */
  public Options() {}

  /**
   * Sets the tolerance.
   *
   * @param tolerance The solver will stop once the error is below this tolerance.
   * @return This Options object.
   */
  public Options withTolerance(double tolerance) {
    this.tolerance = tolerance;
    return this;
  }

  /**
   * Sets the max iterations.
   *
   * @param maxIterations The maximum number of solver iterations before returning a solution.
   * @return This Options object.
   */
  public Options withMaxIterations(int maxIterations) {
    this.maxIterations = maxIterations;
    return this;
  }

  /**
   * Sets the timeout.
   *
   * @param timeout The maximum elapsed wall clock time in seconds before returning a solution.
   * @return This Options object.
   */
  public Options withTimeout(double timeout) {
    this.timeout = timeout;
    return this;
  }

  /**
   * Enables or disables the feasible interior-point method.
   *
   * <p>When the inequality constraints are all feasible, step sizes are reduced when necessary to
   * prevent them becoming infeasible again. This is useful when parts of the problem are
   * ill-conditioned in infeasible regions (e.g., square root of a negative value). This can slow or
   * prevent progress toward a solution though, so only enable it if necessary.
   *
   * @param feasibleIPM Enables or disables the feasible interior-point method.
   * @return This Options object.
   */
  public Options withFeasibleIPM(boolean feasibleIPM) {
    this.feasibleIPM = feasibleIPM;
    return this;
  }

  /**
   * Enables or disables diagnostic output.
   *
   * @param diagnostics Enables or disables diagnostic output.
   * @return This Options object.
   */
  public Options withDiagnostics(boolean diagnostics) {
    this.diagnostics = diagnostics;
    return this;
  }
}
