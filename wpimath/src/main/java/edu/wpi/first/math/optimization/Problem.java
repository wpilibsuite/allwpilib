// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.optimization;

import edu.wpi.first.math.autodiff.ExpressionType;
import edu.wpi.first.math.autodiff.Variable;
import edu.wpi.first.math.autodiff.VariableMatrix;
import edu.wpi.first.math.optimization.solver.ExitStatus;
import edu.wpi.first.math.optimization.solver.IterationInfo;
import edu.wpi.first.math.optimization.solver.Options;
import java.util.ArrayList;
import java.util.function.Predicate;

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
    }
  }

  /**
   * Create a decision variable in the optimization problem.
   *
   * @return A decision variable in the optimization problem.
   */
  public Variable decisionVariable() {
    var handles = ProblemJNI.decisionVariable(m_handle, 1, 1);
    return Variable.fromHandle(handles[0]);
  }

  /**
   * Create a column vector of decision variables in the optimization problem.
   *
   * @param rows Number of matrix rows.
   * @return A matrix of decision variables in the optimization problem.
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
    var handles = ProblemJNI.decisionVariable(m_handle, rows, cols);

    var result = new VariableMatrix(rows, cols);
    for (int i = 0; i < handles.length; ++i) {
      result.set(i, Variable.fromHandle(handles[i]));
    }
    return result;
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
    var handles = ProblemJNI.symmetricDecisionVariable(m_handle, rows);

    var result = new VariableMatrix(rows, rows);
    for (int i = 0; i < handles.length; ++i) {
      result.set(i, Variable.fromHandle(handles[i]));
    }
    return result;
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
    return ExpressionType.fromInteger(ProblemJNI.costFunctionType(m_handle));
  }

  /**
   * Returns the type of the highest order equality constraint.
   *
   * @return The type of the highest order equality constraint.
   */
  public ExpressionType equalityConstraintType() {
    return ExpressionType.fromInteger(ProblemJNI.equalityConstraintType(m_handle));
  }

  /**
   * Returns the type of the highest order inequality constraint.
   *
   * @return The type of the highest order inequality constraint.
   */
  public ExpressionType inequalityConstraintType() {
    return ExpressionType.fromInteger(ProblemJNI.inequalityConstraintType(m_handle));
  }

  /**
   * Solve the optimization problem. The solution will be stored in the original variables used to
   * construct the problem.
   *
   * @return The solver status.
   */
  public ExitStatus solve() {
    return solve(new Options(), false);
  }

  /**
   * Solve the optimization problem. The solution will be stored in the original variables used to
   * construct the problem.
   *
   * @param options Solver options.
   * @return The solver status.
   */
  public ExitStatus solve(Options options) {
    return solve(options, false);
  }

  /**
   * Solve the optimization problem. The solution will be stored in the original variables used to
   * construct the problem.
   *
   * @param options Solver options.
   * @param spy Enables writing sparsity patterns of H, Aₑ, and Aᵢ to files named H.spy, A_e.spy,
   *     and A_i.spy respectively during solve. Use tools/spy.py to plot them.
   * @return The solver status.
   */
  public ExitStatus solve(Options options, boolean spy) {
    return ExitStatus.fromInteger(
        ProblemJNI.solve(
            this,
            m_handle,
            options.tolerance,
            options.maxIterations,
            options.timeout,
            options.feasibleIPM,
            options.diagnostics,
            spy));
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
   * <p>This function is for JNI only.
   *
   * @param info Iteration info.
   * @return True if the solver shold exit early.
   */
  public boolean runCallbacks(IterationInfo info) {
    for (var callback : m_iterationCallbacks) {
      if (callback.test(info)) {
        return true;
      }
    }
    return false;
  }
}
