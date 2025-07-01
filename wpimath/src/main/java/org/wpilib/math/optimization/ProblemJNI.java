// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import org.wpilib.math.jni.WPIMathJNI;

/** Problem JNI functions. */
final class ProblemJNI extends WPIMathJNI {
  private ProblemJNI() {
    // Utility class.
  }

  /** Construct the optimization problem. */
  static native long create();

  /**
   * Destruct the optimization problem.
   *
   * @param handle Problem handle.
   */
  static native void destroy(long handle);

  /**
   * Create a matrix of decision variables in the optimization problem.
   *
   * @param handle Problem handle.
   * @param rows Number of matrix rows.
   * @param cols Number of matrix columns.
   * @return A matrix of decision variables in the optimization problem.
   */
  static native long[] decisionVariable(long handle, int rows, int cols);

  /**
   * Create a symmetric matrix of decision variables in the optimization problem.
   *
   * <p>Variable instances are reused across the diagonal, which helps reduce problem
   * dimensionality.
   *
   * @param handle Problem handle.
   * @param rows Number of matrix rows.
   * @return A symmetric matrix of decision varaibles in the optimization problem.
   */
  static native long[] symmetricDecisionVariable(long handle, int rows);

  /**
   * Tells the solver to minimize the output of the given cost function.
   *
   * <p>Note that this is optional. If only constraints are specified, the solver will find the
   * closest solution to the initial conditions that's in the feasible set.
   *
   * @param handle Problem handle.
   * @param costHandle Variable handle of the cost function to minimize.
   */
  static native void minimize(long handle, long costHandle);

  /**
   * Tells the solver to maximize the output of the given objective function.
   *
   * <p>Note that this is optional. If only constraints are specified, the solver will find the
   * closest solution to the initial conditions that's in the feasible set.
   *
   * @param handle Problem handle.
   * @param objectiveHandle Variable handle of the objective function to maximize.
   */
  static native void maximize(long handle, long objectiveHandle);

  /**
   * Tells the solver to solve the problem while satisfying the given equality constraint.
   *
   * @param handle Problem handle.
   * @param constraintHandles Constraint handles.
   */
  static native void subjectToEq(long handle, long[] constraintHandles);

  /**
   * Tells the solver to solve the problem while satisfying the given inequality constraint.
   *
   * @param handle Problem handle.
   * @param constraintHandles Constraint handles.
   */
  static native void subjectToIneq(long handle, long[] constraintHandles);

  /**
   * Returns the cost function's type.
   *
   * @param handle Problem handle.
   * @return The cost function's type.
   */
  static native int costFunctionType(long handle);

  /**
   * Returns the type of the highest order equality constraint.
   *
   * @param handle Problem handle.
   * @return The type of the highest order equality constraint.
   */
  static native int equalityConstraintType(long handle);

  /**
   * Returns the type of the highest order inequality constraint.
   *
   * @param handle Problem handle.
   * @return The type of the highest order inequality constraint.
   */
  static native int inequalityConstraintType(long handle);

  /**
   * Solve the optimization problem. The solution will be stored in the original variables used to
   * construct the problem.
   *
   * @param obj Java Problem object.
   * @param handle Problem handle.
   * @param tolerance The solver will stop once the error is below this tolerance.
   * @param maxIterations The maximum number of solver iterations before returning a solution.
   * @param timeout The maximum elapsed wall clock time in seconds before returning a solution.
   * @param feasibleIPM Enables the feasible interior-point method. When the inequality constraints
   *     are all feasible, step sizes are reduced when necessary to prevent them becoming infeasible
   *     again. This is useful when parts of the problem are ill-conditioned in infeasible regions
   *     (e.g., square root of a negative value). This can slow or prevent progress toward a
   *     solution though, so only enable it if necessary.
   * @param diagnnostics Enables diagnostic prints.
   * @return The solver status.
   */
  static native int solve(
      Problem obj,
      long handle,
      double tolerance,
      int maxIterations,
      double timeout,
      boolean feasibleIPM,
      boolean diagnostics);
}
