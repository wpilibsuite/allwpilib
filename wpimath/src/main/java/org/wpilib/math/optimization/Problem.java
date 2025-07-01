// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import java.util.ArrayList;
import java.util.function.Predicate;
import org.ejml.data.DMatrixRMaj;
import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.autodiff.ExpressionType;
import org.wpilib.math.autodiff.NativeSparseTriplets;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableMatrix;
import org.wpilib.math.autodiff.VariablePool;
import org.wpilib.math.optimization.solver.ExitStatus;
import org.wpilib.math.optimization.solver.IterationInfo;
import org.wpilib.math.optimization.solver.Options;

/**
 * This class allows the user to pose a constrained nonlinear optimization problem in natural
 * mathematical notation and solve it.
 *
 * <p>This class supports problems of the form:
 *
 * <pre>
 *       minₓ f(x)
 * subject to cₑ(x) = 0
 *            cᵢ(x) ≥ 0
 * </pre>
 *
 * <p>where f(x) is the scalar cost function, x is the vector of decision variables (variables the
 * solver can tweak to minimize the cost function), cᵢ(x) are the inequality constraints, and cₑ(x)
 * are the equality constraints. Constraints are equations or inequalities of the decision variables
 * that constrain what values the solver is allowed to use when searching for an optimal solution.
 *
 * <p>The nice thing about this class is users don't have to put their system in the form shown
 * above manually; they can write it in natural mathematical form and it'll be converted for them.
 */
public class Problem implements AutoCloseable {
  private long m_handle;

  // The iteration callbacks
  private final ArrayList<Predicate<IterationInfo>> m_iterationCallbacks = new ArrayList<>();

  // Cleans up Variables allocated within Problem's scope
  private final VariablePool m_pool = new VariablePool();

  /** Construct the optimization problem. */
  @SuppressWarnings("this-escape")
  public Problem() {
    m_handle = ProblemJNI.create();
  }

  @Override
  public void close() {
    if (m_handle != 0) {
      ProblemJNI.destroy(m_handle);
      m_handle = 0;

      m_pool.close();
    }
  }

  /**
   * Create a decision variable in the optimization problem.
   *
   * @return A decision variable in the optimization problem.
   */
  public Variable decisionVariable() {
    var handles = ProblemJNI.decisionVariable(m_handle, 1, 1);
    return new Variable(Variable.HANDLE, handles[0]);
  }

  /**
   * Create a column vector of decision variables in the optimization problem.
   *
   * @param rows Number of column vector rows.
   * @return A column vector of decision variables in the optimization problem.
   */
  public VariableMatrix decisionVariable(int rows) {
    return decisionVariable(rows, 1);
  }

  /**
   * Create a matrix of decision variables in the optimization problem.
   *
   * @param rows Number of matrix rows.
   * @param cols Number of matrix columns.
   * @return A matrix of decision variables in the optimization problem.
   */
  public VariableMatrix decisionVariable(int rows, int cols) {
    return new VariableMatrix(rows, cols, ProblemJNI.decisionVariable(m_handle, rows, cols));
  }

  /**
   * Create a symmetric matrix of decision variables in the optimization problem.
   *
   * <p>Variable instances are reused across the diagonal, which helps reduce problem
   * dimensionality.
   *
   * @param rows Number of matrix rows.
   * @return A symmetric matrix of decision varaibles in the optimization problem.
   */
  public VariableMatrix symmetricDecisionVariable(int rows) {
    return new VariableMatrix(rows, rows, ProblemJNI.symmetricDecisionVariable(m_handle, rows));
  }

  /**
   * Tells the solver to minimize the output of the given cost function.
   *
   * <p>Note that this is optional. If only constraints are specified, the solver will find the
   * closest solution to the initial conditions that's in the feasible set.
   *
   * @param cost The cost function to minimize.
   */
  public void minimize(Variable cost) {
    ProblemJNI.minimize(m_handle, cost.getHandle());
  }

  /**
   * Tells the solver to minimize the output of the given cost function.
   *
   * <p>Note that this is optional. If only constraints are specified, the solver will find the
   * closest solution to the initial conditions that's in the feasible set.
   *
   * @param cost The cost function to minimize. An assertion is raised if the VariableMatrix isn't
   *     1x1.
   */
  public void minimize(VariableMatrix cost) {
    assert cost.rows() == 1 && cost.cols() == 1;
    minimize(cost.get(0, 0));
  }

  /**
   * Tells the solver to maximize the output of the given objective function.
   *
   * <p>Note that this is optional. If only constraints are specified, the solver will find the
   * closest solution to the initial conditions that's in the feasible set.
   *
   * @param objective The objective function to maximize.
   */
  public void maximize(Variable objective) {
    ProblemJNI.maximize(m_handle, objective.getHandle());
  }

  /**
   * Tells the solver to maximize the output of the given objective function.
   *
   * <p>Note that this is optional. If only constraints are specified, the solver will find the
   * closest solution to the initial conditions that's in the feasible set.
   *
   * @param objective The objective function to maximize. An assertion is raised if the
   *     VariableMatrix isn't 1x1.
   */
  public void maximize(VariableMatrix objective) {
    assert objective.rows() == 1 && objective.cols() == 1;
    maximize(objective.get(0, 0));
  }

  /**
   * Tells the solver to solve the problem while satisfying the given equality constraint.
   *
   * @param constraint The constraint to satisfy.
   */
  public void subjectTo(EqualityConstraints constraint) {
    var constraintHandles = new long[constraint.constraints.length];
    for (int i = 0; i < constraintHandles.length; ++i) {
      constraintHandles[i] = constraint.constraints[i].getHandle();
    }
    ProblemJNI.subjectToEq(m_handle, constraintHandles);
  }

  /**
   * Tells the solver to solve the problem while satisfying the given inequality constraint.
   *
   * @param constraint The constraint to satisfy.
   */
  public void subjectTo(InequalityConstraints constraint) {
    var constraintHandles = new long[constraint.constraints.length];
    for (int i = 0; i < constraintHandles.length; ++i) {
      constraintHandles[i] = constraint.constraints[i].getHandle();
    }
    ProblemJNI.subjectToIneq(m_handle, constraintHandles);
  }

  /**
   * Returns the cost function's type.
   *
   * @return The cost function's type.
   */
  public ExpressionType costFunctionType() {
    return ExpressionType.fromInt(ProblemJNI.costFunctionType(m_handle));
  }

  /**
   * Returns the type of the highest order equality constraint.
   *
   * @return The type of the highest order equality constraint.
   */
  public ExpressionType equalityConstraintType() {
    return ExpressionType.fromInt(ProblemJNI.equalityConstraintType(m_handle));
  }

  /**
   * Returns the type of the highest order inequality constraint.
   *
   * @return The type of the highest order inequality constraint.
   */
  public ExpressionType inequalityConstraintType() {
    return ExpressionType.fromInt(ProblemJNI.inequalityConstraintType(m_handle));
  }

  /**
   * Solve the optimization problem. The solution will be stored in the original variables used to
   * construct the problem.
   *
   * @return The solver status.
   */
  public ExitStatus solve() {
    return solve(new Options());
  }

  /**
   * Solve the optimization problem. The solution will be stored in the original variables used to
   * construct the problem.
   *
   * @param options Solver options.
   * @return The solver status.
   */
  public ExitStatus solve(Options options) {
    return ExitStatus.fromInt(
        ProblemJNI.solve(
            this,
            m_handle,
            options.tolerance,
            options.maxIterations,
            options.timeout,
            options.feasibleIPM,
            options.diagnostics));
  }

  /**
   * Adds a callback to be called at the beginning of each solver iteration.
   *
   * <p>The callback for this overload should return bool.
   *
   * @param callback The callback. Returning true from the callback causes the solver to exit early
   *     with the solution it has so far.
   */
  public void addCallback(Predicate<IterationInfo> callback) {
    m_iterationCallbacks.add(callback);
  }

  /** Clears the registered callbacks. */
  public void clearCallbacks() {
    m_iterationCallbacks.clear();
  }

  /**
   * Runs the registered callbacks.
   *
   * <p>This function is called by native code in ProblemJNI.
   *
   * @param numEqualityConstraints The number of equality constraints.
   * @param numInequalityConstraints The number of inequality constraints.
   * @param iteration The solver iteration.
   * @param x The decision variable values.
   * @param gTriplets Gradient triplets.
   * @param HTriplets Hessian triplets.
   * @param A_eTriplets Equality constraint Jacobian triplets.
   * @param A_iTriplets Inequality constraint Jacobian triplets.
   * @return True if the solver shold exit early.
   */
  boolean runCallbacks(
      int numEqualityConstraints,
      int numInequalityConstraints,
      int iteration,
      double[] x,
      NativeSparseTriplets gTriplets,
      NativeSparseTriplets HTriplets,
      NativeSparseTriplets A_eTriplets,
      NativeSparseTriplets A_iTriplets) {
    if (m_iterationCallbacks.isEmpty()) {
      return false;
    }

    var info =
        new IterationInfo(
            iteration,
            new SimpleMatrix(DMatrixRMaj.wrap(x.length, 1, x)),
            gTriplets.toSimpleMatrix(x.length, 1),
            HTriplets.toSimpleMatrix(x.length, x.length),
            A_eTriplets.toSimpleMatrix(numEqualityConstraints, x.length),
            A_iTriplets.toSimpleMatrix(numInequalityConstraints, x.length));

    for (var callback : m_iterationCallbacks) {
      if (callback.test(info)) {
        return true;
      }
    }
    return false;
  }
}
