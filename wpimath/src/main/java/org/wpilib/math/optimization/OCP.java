// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.wpilib.math.optimization.Constraints.eq;
import static org.wpilib.math.optimization.Constraints.ge;
import static org.wpilib.math.optimization.Constraints.le;

import java.util.function.BiConsumer;
import java.util.function.BiFunction;
import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableBlock;
import org.wpilib.math.autodiff.VariableMatrix;
import org.wpilib.math.optimization.ocp.ConstraintEvaluationFunction;
import org.wpilib.math.optimization.ocp.DynamicsFunction;
import org.wpilib.math.optimization.ocp.DynamicsType;
import org.wpilib.math.optimization.ocp.TimestepMethod;
import org.wpilib.math.optimization.ocp.TranscriptionMethod;

/**
 * This class allows the user to pose and solve a constrained optimal control problem (OCP) in a
 * variety of ways.
 *
 * <p>The system is transcripted by one of three methods (direct transcription, direct collocation,
 * or single-shooting) and additional constraints can be added.
 *
 * <p>In direct transcription, each state is a decision variable constrained to the integrated
 * dynamics of the previous state. In direct collocation, the trajectory is modeled as a series of
 * cubic polynomials where the centerpoint slope is constrained. In single-shooting, states depend
 * explicitly as a function of all previous states and all previous inputs.
 *
 * <p>Explicit ODEs are integrated using RK4.
 *
 * <p>For explicit ODEs, the function must be in the form dx/dt = f(t, x, u). For discrete state
 * transition functions, the function must be in the form xₖ₊₁ = f(t, xₖ, uₖ).
 *
 * <p>Direct collocation requires an explicit ODE. Direct transcription and single-shooting can use
 * either an ODE or state transition function.
 *
 * <p>https://underactuated.mit.edu/trajopt.html goes into more detail on each transcription method.
 */
public class OCP extends Problem {
  private int m_numSteps;

  private DynamicsFunction m_dynamics;
  private DynamicsType m_dynamicsType;

  private VariableMatrix m_X;
  private VariableMatrix m_U;
  private VariableMatrix m_DT;

  /**
   * Build an optimization problem using a system evolution function (explicit ODE or discrete state
   * transition function).
   *
   * @param numStates The number of system states.
   * @param numInputs The number of system inputs.
   * @param dt The timestep for fixed-step integration.
   * @param numSteps The number of control points.
   * @param dynamics Function representing an explicit or implicit ODE, or a discrete state
   *     transition function.
   *     <ul>
   *       <li>Explicit: dx/dt = f(x, u, *)
   *       <li>Implicit: f([x dx/dt]', u, *) = 0
   *       <li>State transition: xₖ₊₁ = f(xₖ, uₖ)
   *     </ul>
   *
   * @param dynamicsType The type of system evolution function.
   * @param timestepMethod The timestep method.
   * @param transcriptionMethod The transcription method.
   */
  public OCP(
      int numStates,
      int numInputs,
      double dt,
      int numSteps,
      BiFunction<VariableMatrix, VariableMatrix, VariableMatrix> dynamics,
      DynamicsType dynamicsType,
      TimestepMethod timestepMethod,
      TranscriptionMethod transcriptionMethod) {
    this(
        numStates,
        numInputs,
        dt,
        numSteps,
        (Variable t, VariableMatrix x, VariableMatrix u, Variable _dt) -> dynamics.apply(x, u),
        dynamicsType,
        timestepMethod,
        transcriptionMethod);
  }

  /**
   * Build an optimization problem using a system evolution function (explicit ODE or discrete state
   * transition function).
   *
   * @param numStates The number of system states.
   * @param numInputs The number of system inputs.
   * @param dt The timestep for fixed-step integration.
   * @param numSteps The number of control points.
   * @param dynamics Function representing an explicit or implicit ODE, or a discrete state
   *     transition function.
   *     <ul>
   *       <li>Explicit: dx/dt = f(t, x, u, *)
   *       <li>Implicit: f(t, [x dx/dt]', u, *) = 0
   *       <li>State transition: xₖ₊₁ = f(t, xₖ, uₖ, dt)
   *     </ul>
   *
   * @param dynamicsType The type of system evolution function.
   * @param timestepMethod The timestep method.
   * @param transcriptionMethod The transcription method.
   */
  @SuppressWarnings("this-escape")
  public OCP(
      int numStates,
      int numInputs,
      double dt,
      int numSteps,
      DynamicsFunction dynamics,
      DynamicsType dynamicsType,
      TimestepMethod timestepMethod,
      TranscriptionMethod transcriptionMethod) {
    m_numSteps = numSteps;
    m_dynamics = dynamics;
    m_dynamicsType = dynamicsType;

    // u is numSteps + 1 so that the final constraint function evaluation works
    m_U = decisionVariable(numInputs, m_numSteps + 1);

    if (timestepMethod == TimestepMethod.FIXED) {
      m_DT = new VariableMatrix(1, m_numSteps + 1);
      for (int i = 0; i < numSteps + 1; ++i) {
        m_DT.set(0, i, dt);
      }
    } else if (timestepMethod == TimestepMethod.VARIABLE_SINGLE) {
      Variable single_dt = decisionVariable();
      single_dt.setValue(dt);

      // Set the member variable matrix to track the decision variable
      m_DT = new VariableMatrix(1, m_numSteps + 1);
      for (int i = 0; i < numSteps + 1; ++i) {
        m_DT.set(0, i, single_dt);
      }
    } else if (timestepMethod == TimestepMethod.VARIABLE) {
      m_DT = decisionVariable(1, m_numSteps + 1);
      for (int i = 0; i < numSteps + 1; ++i) {
        m_DT.get(0, i).setValue(dt);
      }
    }

    if (transcriptionMethod == TranscriptionMethod.DIRECT_TRANSCRIPTION) {
      m_X = decisionVariable(numStates, m_numSteps + 1);
      constrainDirectTranscription();
    } else if (transcriptionMethod == TranscriptionMethod.DIRECT_COLLOCATION) {
      m_X = decisionVariable(numStates, m_numSteps + 1);
      constrainDirectCollocation();
    } else if (transcriptionMethod == TranscriptionMethod.SINGLE_SHOOTING) {
      // In single-shooting the states aren't decision variables, but instead
      // depend on the input and previous states
      m_X = new VariableMatrix(numStates, m_numSteps + 1);
      constrainSingleShooting();
    }
  }

  /**
   * Utility function to constrain the initial state.
   *
   * @param initialState the initial state to constrain to.
   */
  public void constrainInitialState(double initialState) {
    subjectTo(eq(this.initialState(), initialState));
  }

  /**
   * Utility function to constrain the initial state.
   *
   * @param initialState the initial state to constrain to.
   */
  public void constrainInitialState(Variable initialState) {
    subjectTo(eq(this.initialState(), initialState));
  }

  /**
   * Utility function to constrain the initial state.
   *
   * @param initialState the initial state to constrain to.
   */
  public void constrainInitialState(SimpleMatrix initialState) {
    subjectTo(eq(this.initialState(), initialState));
  }

  /**
   * Utility function to constrain the initial state.
   *
   * @param initialState the initial state to constrain to.
   */
  public void constrainInitialState(VariableMatrix initialState) {
    subjectTo(eq(this.initialState(), initialState));
  }

  /**
   * Utility function to constrain the initial state.
   *
   * @param initialState the initial state to constrain to.
   */
  public void constrainInitialState(VariableBlock initialState) {
    subjectTo(eq(this.initialState(), initialState));
  }

  /**
   * Utility function to constrain the final state.
   *
   * @param finalState the final state to constrain to.
   */
  public void constrainFinalState(double finalState) {
    subjectTo(eq(this.finalState(), finalState));
  }

  /**
   * Utility function to constrain the final state.
   *
   * @param finalState the final state to constrain to.
   */
  public void constrainFinalState(Variable finalState) {
    subjectTo(eq(this.finalState(), finalState));
  }

  /**
   * Utility function to constrain the final state.
   *
   * @param finalState the final state to constrain to.
   */
  public void constrainFinalState(SimpleMatrix finalState) {
    subjectTo(eq(this.finalState(), finalState));
  }

  /**
   * Utility function to constrain the final state.
   *
   * @param finalState the final state to constrain to.
   */
  public void constrainFinalState(VariableMatrix finalState) {
    subjectTo(eq(this.finalState(), finalState));
  }

  /**
   * Utility function to constrain the final state.
   *
   * @param finalState the final state to constrain to.
   */
  public void constrainFinalState(VariableBlock finalState) {
    subjectTo(eq(this.finalState(), finalState));
  }

  /**
   * Set the constraint evaluation function. This function is called `numSteps+1` times, with the
   * corresponding state and input VariableMatrices.
   *
   * @param callback The callback f(x, u) where x is the state and u is the input vector.
   */
  public void forEachStep(BiConsumer<VariableMatrix, VariableMatrix> callback) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      var x = X().col(i);
      var u = U().col(i);
      callback.accept(new VariableMatrix(x), new VariableMatrix(u));
    }
  }

  /**
   * Set the constraint evaluation function. This function is called `numSteps+1` times, with the
   * corresponding state and input VariableMatrices.
   *
   * @param callback The callback f(t, x, u, dt) where t is time, x is the state vector, u is the
   *     input vector, and dt is the timestep duration.
   */
  public void forEachStep(ConstraintEvaluationFunction callback) {
    var time = new Variable(0.0);

    for (int i = 0; i < m_numSteps + 1; ++i) {
      var x = X().col(i);
      var u = U().col(i);
      var dt = this.dt().get(0, i);
      callback.accept(time, new VariableMatrix(x), new VariableMatrix(u), dt);

      time = time.plus(dt);
    }
  }

  /**
   * Convenience function to set a lower bound on the input.
   *
   * @param lowerBound The lower bound that inputs must always be above. Must be shaped
   *     (numInputs)x1.
   */
  public void setLowerInputBound(double lowerBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      subjectTo(ge(U().col(i), lowerBound));
    }
  }

  /**
   * Convenience function to set a lower bound on the input.
   *
   * @param lowerBound The lower bound that inputs must always be above. Must be shaped
   *     (numInputs)x1.
   */
  public void setLowerInputBound(Variable lowerBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      subjectTo(ge(U().col(i), lowerBound));
    }
  }

  /**
   * Convenience function to set a lower bound on the input.
   *
   * @param lowerBound The lower bound that inputs must always be above. Must be shaped
   *     (numInputs)x1.
   */
  public void setLowerInputBound(SimpleMatrix lowerBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      subjectTo(ge(U().col(i), lowerBound));
    }
  }

  /**
   * Convenience function to set a lower bound on the input.
   *
   * @param lowerBound The lower bound that inputs must always be above. Must be shaped
   *     (numInputs)x1.
   */
  public void setLowerInputBound(VariableMatrix lowerBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      subjectTo(ge(U().col(i), lowerBound));
    }
  }

  /**
   * Convenience function to set a lower bound on the input.
   *
   * @param lowerBound The lower bound that inputs must always be above. Must be shaped
   *     (numInputs)x1.
   */
  public void setLowerInputBound(VariableBlock lowerBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      subjectTo(ge(U().col(i), lowerBound));
    }
  }

  /**
   * Convenience function to set an upper bound on the input.
   *
   * @param upperBound The upper bound that inputs must always be below. Must be shaped
   *     (numInputs)x1.
   */
  public void setUpperInputBound(double upperBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      subjectTo(le(U().col(i), upperBound));
    }
  }

  /**
   * Convenience function to set an upper bound on the input.
   *
   * @param upperBound The upper bound that inputs must always be below. Must be shaped
   *     (numInputs)x1.
   */
  public void setUpperInputBound(Variable upperBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      subjectTo(le(U().col(i), upperBound));
    }
  }

  /**
   * Convenience function to set an upper bound on the input.
   *
   * @param upperBound The upper bound that inputs must always be below. Must be shaped
   *     (numInputs)x1.
   */
  public void setUpperInputBound(SimpleMatrix upperBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      subjectTo(le(U().col(i), upperBound));
    }
  }

  /**
   * Convenience function to set an upper bound on the input.
   *
   * @param upperBound The upper bound that inputs must always be below. Must be shaped
   *     (numInputs)x1.
   */
  public void setUpperInputBound(VariableMatrix upperBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      subjectTo(le(U().col(i), upperBound));
    }
  }

  /**
   * Convenience function to set an upper bound on the input.
   *
   * @param upperBound The upper bound that inputs must always be below. Must be shaped
   *     (numInputs)x1.
   */
  public void setUpperInputBound(VariableBlock upperBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      subjectTo(le(U().col(i), upperBound));
    }
  }

  /**
   * Convenience function to set a lower bound on the timestep.
   *
   * @param minTimestep The minimum timestep in seconds.
   */
  public void setMinTimestep(double minTimestep) {
    subjectTo(ge(dt(), minTimestep));
  }

  /**
   * Convenience function to set an upper bound on the timestep.
   *
   * @param maxTimestep The maximum timestep in seconds.
   */
  public void setMaxTimestep(double maxTimestep) {
    subjectTo(le(dt(), maxTimestep));
  }

  /**
   * Get the state variables. After the problem is solved, this will contain the optimized
   * trajectory.
   *
   * <p>Shaped (numStates)x(numSteps+1).
   *
   * @return The state variable matrix.
   */
  public VariableMatrix X() {
    return m_X;
  }

  /**
   * Get the input variables. After the problem is solved, this will contain the inputs
   * corresponding to the optimized trajectory.
   *
   * <p>Shaped (numInputs)x(numSteps+1), although the last input step is unused in the trajectory.
   *
   * @return The input variable matrix.
   */
  public VariableMatrix U() {
    return m_U;
  }

  /**
   * Get the timestep variables. After the problem is solved, this will contain the timesteps
   * corresponding to the optimized trajectory.
   *
   * <p>Shaped 1x(numSteps+1), although the last timestep is unused in the trajectory.
   *
   * @return The timestep variable matrix.
   */
  public VariableMatrix dt() {
    return m_DT;
  }

  /**
   * Convenience function to get the initial state in the trajectory.
   *
   * @return The initial state of the trajectory.
   */
  public VariableMatrix initialState() {
    return new VariableMatrix(m_X.col(0));
  }

  /**
   * Convenience function to get the final state in the trajectory.
   *
   * @return The final state of the trajectory.
   */
  public VariableMatrix finalState() {
    return new VariableMatrix(m_X.col(m_numSteps));
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(t, x, u) for dt.
   *
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param t0 The initial time.
   * @param dt The time over which to integrate.
   */
  private static VariableMatrix rk4(
      DynamicsFunction f, VariableMatrix x, VariableMatrix u, Variable t0, Variable dt) {
    var halfdt = dt.times(0.5);
    VariableMatrix k1 = f.apply(t0, x, u, dt);
    VariableMatrix k2 = f.apply(t0.plus(halfdt), x.plus(k1.times(halfdt)), u, dt);
    VariableMatrix k3 = f.apply(t0.plus(halfdt), x.plus(k2.times(halfdt)), u, dt);
    VariableMatrix k4 = f.apply(t0.plus(dt), x.plus(k3.times(dt)), u, dt);

    return x.plus(k1.plus(k2.times(2.0)).plus(k3.times(2.0)).plus(k4).times(dt.div(6.0)));
  }

  /** Apply direct collocation dynamics constraints. */
  private void constrainDirectCollocation() {
    assert m_dynamicsType == DynamicsType.EXPLICIT_ODE;

    var time = new Variable(0.0);

    // Derivation at https://mec560sbu.github.io/2016/09/30/direct_collocation/
    for (int i = 0; i < m_numSteps; ++i) {
      Variable h = dt().get(0, i);

      var f = m_dynamics;

      var t_begin = time;
      var t_end = t_begin.plus(h);

      var x_begin = X().col(i);
      var x_end = X().col(i + 1);

      var u_begin = U().col(i);
      var u_end = U().col(i + 1);

      var xdot_begin =
          f.apply(t_begin, new VariableMatrix(x_begin), new VariableMatrix(u_begin), h);
      var xdot_end = f.apply(t_end, new VariableMatrix(x_end), new VariableMatrix(u_end), h);
      var xdot_c =
          x_begin
              .minus(x_end)
              .times(new Variable(-3).div(h.times(2)))
              .minus(xdot_begin.plus(xdot_end).times(0.25));

      var t_c = t_begin.plus(h.times(0.5));
      var x_c = x_begin.plus(x_end).times(0.5).plus(xdot_begin.minus(xdot_end).times(h.div(8)));
      var u_c = u_begin.plus(u_end).times(0.5);

      subjectTo(eq(xdot_c, f.apply(t_c, x_c, u_c, h)));

      time = time.plus(h);
    }
  }

  /** Apply direct transcription dynamics constraints. */
  private void constrainDirectTranscription() {
    var time = new Variable(0.0);

    for (int i = 0; i < m_numSteps; ++i) {
      var x_begin = X().col(i);
      var x_end = X().col(i + 1);
      var u = U().col(i);
      Variable dt = this.dt().get(0, i);

      if (m_dynamicsType == DynamicsType.EXPLICIT_ODE) {
        subjectTo(
            eq(
                x_end,
                rk4(m_dynamics, new VariableMatrix(x_begin), new VariableMatrix(u), time, dt)));
      } else if (m_dynamicsType == DynamicsType.DISCRETE) {
        subjectTo(
            eq(
                x_end,
                m_dynamics.apply(time, new VariableMatrix(x_begin), new VariableMatrix(u), dt)));
      }

      time = time.plus(dt);
    }
  }

  /** Apply single shooting dynamics constraints. */
  private void constrainSingleShooting() {
    var time = new Variable(0.0);

    for (int i = 0; i < m_numSteps; ++i) {
      var x_begin = X().col(i);
      var x_end = X().col(i + 1);
      var u = U().col(i);
      Variable dt = this.dt().get(0, i);

      if (m_dynamicsType == DynamicsType.EXPLICIT_ODE) {
        x_end.set(rk4(m_dynamics, new VariableMatrix(x_begin), new VariableMatrix(u), time, dt));
      } else if (m_dynamicsType == DynamicsType.DISCRETE) {
        x_end.set(m_dynamics.apply(time, new VariableMatrix(x_begin), new VariableMatrix(u), dt));
      }

      time = time.plus(dt);
    }
  }
}
