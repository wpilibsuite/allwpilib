// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization.solver;

/** Solver exit status. Negative values indicate failure. */
public enum ExitStatus {
  /** Solved the problem to the desired tolerance. */
  SUCCESS(0),
  /** The solver returned its solution so far after the user requested a stop. */
  CALLBACK_REQUESTED_STOP(1),
  /** The solver determined the problem to be overconstrained and gave up. */
  TOO_FEW_DOFS(-1),
  /** The solver determined the problem to be locally infeasible and gave up. */
  LOCALLY_INFEASIBLE(-2),
  /** The problem setup frontend determined the problem to have an empty feasible region. */
  GLOBALLY_INFEASIBLE(-3),
  /** The linear system factorization failed. */
  FACTORIZATION_FAILED(-4),
  /** The backtracking line search failed, and the problem isn't locally infeasible. */
  LINE_SEARCH_FAILED(-5),
  /** The solver encountered nonfinite initial cost or constraints and gave up. */
  NONFINITE_INITIAL_COST_OR_CONSTRAINTS(-6),
  /** The solver encountered diverging primal iterates xₖ and/or sₖ and gave up. */
  DIVERGING_ITERATES(-7),
  /** The solver returned its solution so far after exceeding the maximum number of iterations. */
  MAX_ITERATIONS_EXCEEDED(-8),
  /**
   * The solver returned its solution so far after exceeding the maximum elapsed wall clock time.
   */
  TIMEOUT(-9);

  /** ExitStatus value. */
  public final int value;

  ExitStatus(int value) {
    this.value = value;
  }

  /**
   * Converts integer to its corresponding enum value.
   *
   * @param x The integer.
   * @return The enum value.
   */
  public static ExitStatus fromInt(int x) {
    return switch (x) {
      case 0 -> ExitStatus.SUCCESS;
      case 1 -> ExitStatus.CALLBACK_REQUESTED_STOP;
      case -1 -> ExitStatus.TOO_FEW_DOFS;
      case -2 -> ExitStatus.LOCALLY_INFEASIBLE;
      case -3 -> ExitStatus.GLOBALLY_INFEASIBLE;
      case -4 -> ExitStatus.FACTORIZATION_FAILED;
      case -5 -> ExitStatus.LINE_SEARCH_FAILED;
      case -6 -> ExitStatus.NONFINITE_INITIAL_COST_OR_CONSTRAINTS;
      case -7 -> ExitStatus.DIVERGING_ITERATES;
      case -8 -> ExitStatus.MAX_ITERATIONS_EXCEEDED;
      case -9 -> ExitStatus.TIMEOUT;
      default -> null;
    };
  }
}
