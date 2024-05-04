// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

#include <chrono>
#include <utility>

#include "sleipnir/autodiff/VariableMatrix.hpp"
#include "sleipnir/optimization/OptimizationProblem.hpp"
#include "sleipnir/util/Assert.hpp"
#include "sleipnir/util/Concepts.hpp"
#include "sleipnir/util/FunctionRef.hpp"
#include "sleipnir/util/SymbolExports.hpp"

namespace sleipnir {

/**
 * Function representing an explicit or implicit ODE, or a discrete state
 * transition function.
 *
 * - Explicit: dx/dt = f(t, x, u, *)
 * - Implicit: f(t, [x dx/dt]', u, *) = 0
 * - State transition: xₖ₊₁ = f(t, xₖ, uₖ, dt)
 */
using DynamicsFunction =
    function_ref<VariableMatrix(const Variable&, const VariableMatrix&,
                                const VariableMatrix&, const Variable&)>;

/**
 * Performs 4th order Runge-Kutta integration of dx/dt = f(t, x, u) for dt.
 *
 * @param f  The function to integrate. It must take two arguments x and u.
 * @param x  The initial value of x.
 * @param u  The value u held constant over the integration period.
 * @param t0 The initial time.
 * @param dt The time over which to integrate.
 */
template <typename F, typename State, typename Input, typename Time>
State RK4(F&& f, State x, Input u, Time t0, Time dt) {
  auto halfdt = dt * 0.5;
  State k1 = f(t0, x, u, dt);
  State k2 = f(t0 + halfdt, x + k1 * halfdt, u, dt);
  State k3 = f(t0 + halfdt, x + k2 * halfdt, u, dt);
  State k4 = f(t0 + dt, x + k3 * dt, u, dt);

  return x + (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (dt / 6.0);
}

/**
 * Enum describing an OCP transcription method.
 */
enum class TranscriptionMethod : uint8_t {
  /// Each state is a decision variable constrained to the integrated dynamics
  /// of the previous state.
  kDirectTranscription,
  /// The trajectory is modeled as a series of cubic polynomials where the
  /// centerpoint slope is constrained.
  kDirectCollocation,
  /// States depend explicitly as a function of all previous states and all
  /// previous inputs.
  kSingleShooting
};

/**
 * Enum describing a type of system dynamics constraints.
 */
enum class DynamicsType : uint8_t {
  /// The dynamics are a function in the form dx/dt = f(t, x, u).
  kExplicitODE,
  /// The dynamics are a function in the form xₖ₊₁ = f(t, xₖ, uₖ).
  kDiscrete
};

/**
 * Enum describing the type of system timestep.
 */
enum class TimestepMethod : uint8_t {
  /// The timestep is a fixed constant.
  kFixed,
  /// The timesteps are allowed to vary as independent decision variables.
  kVariable,
  /// The timesteps are equal length but allowed to vary as a single decision
  /// variable.
  kVariableSingle
};

/**
 * This class allows the user to pose and solve a constrained optimal control
 * problem (OCP) in a variety of ways.
 *
 * The system is transcripted by one of three methods (direct transcription,
 * direct collocation, or single-shooting) and additional constraints can be
 * added.
 *
 * In direct transcription, each state is a decision variable constrained to the
 * integrated dynamics of the previous state. In direct collocation, the
 * trajectory is modeled as a series of cubic polynomials where the centerpoint
 * slope is constrained. In single-shooting, states depend explicitly as a
 * function of all previous states and all previous inputs.
 *
 * Explicit ODEs are integrated using RK4.
 *
 * For explicit ODEs, the function must be in the form dx/dt = f(t, x, u).
 * For discrete state transition functions, the function must be in the form
 * xₖ₊₁ = f(t, xₖ, uₖ).
 *
 * Direct collocation requires an explicit ODE. Direct transcription and
 * single-shooting can use either an ODE or state transition function.
 *
 * https://underactuated.mit.edu/trajopt.html goes into more detail on each
 * transcription method.
 */
class SLEIPNIR_DLLEXPORT OCPSolver : public OptimizationProblem {
 public:
  /**
   * Build an optimization problem using a system evolution function (explicit
   * ODE or discrete state transition function).
   *
   * @param numStates The number of system states.
   * @param numInputs The number of system inputs.
   * @param dt The timestep for fixed-step integration.
   * @param numSteps The number of control points.
   * @param dynamics The system evolution function, either an explicit ODE or a
   * discrete state transition function.
   * @param dynamicsType The type of system evolution function.
   * @param timestepMethod The timestep method.
   * @param method The transcription method.
   */
  OCPSolver(
      int numStates, int numInputs, std::chrono::duration<double> dt,
      int numSteps, DynamicsFunction dynamics,
      DynamicsType dynamicsType = DynamicsType::kExplicitODE,
      TimestepMethod timestepMethod = TimestepMethod::kFixed,
      TranscriptionMethod method = TranscriptionMethod::kDirectTranscription)
      : m_numStates{numStates},
        m_numInputs{numInputs},
        m_dt{dt},
        m_numSteps{numSteps},
        m_transcriptionMethod{method},
        m_dynamicsType{dynamicsType},
        m_dynamicsFunction{std::move(dynamics)},
        m_timestepMethod{timestepMethod} {
    // u is numSteps + 1 so that the final constraintFunction evaluation works
    m_U = DecisionVariable(m_numInputs, m_numSteps + 1);

    if (m_timestepMethod == TimestepMethod::kFixed) {
      m_DT = VariableMatrix{1, m_numSteps + 1};
      for (int i = 0; i < numSteps + 1; ++i) {
        m_DT(0, i) = m_dt.count();
      }
    } else if (m_timestepMethod == TimestepMethod::kVariableSingle) {
      Variable DT = DecisionVariable();
      DT.SetValue(m_dt.count());

      // Set the member variable matrix to track the decision variable
      m_DT = VariableMatrix{1, m_numSteps + 1};
      for (int i = 0; i < numSteps + 1; ++i) {
        m_DT(0, i) = DT;
      }
    } else if (m_timestepMethod == TimestepMethod::kVariable) {
      m_DT = DecisionVariable(1, m_numSteps + 1);
      for (int i = 0; i < numSteps + 1; ++i) {
        m_DT(0, i).SetValue(m_dt.count());
      }
    }

    if (m_transcriptionMethod == TranscriptionMethod::kDirectTranscription) {
      m_X = DecisionVariable(m_numStates, m_numSteps + 1);
      ConstrainDirectTranscription();
    } else if (m_transcriptionMethod ==
               TranscriptionMethod::kDirectCollocation) {
      m_X = DecisionVariable(m_numStates, m_numSteps + 1);
      ConstrainDirectCollocation();
    } else if (m_transcriptionMethod == TranscriptionMethod::kSingleShooting) {
      // In single-shooting the states aren't decision variables, but instead
      // depend on the input and previous states
      m_X = VariableMatrix{m_numStates, m_numSteps + 1};
      ConstrainSingleShooting();
    }
  }

  /**
   * Utility function to constrain the initial state.
   *
   * @param initialState the initial state to constrain to.
   */
  template <typename T>
    requires ScalarLike<T> || MatrixLike<T>
  void ConstrainInitialState(const T& initialState) {
    SubjectTo(InitialState() == initialState);
  }

  /**
   * Utility function to constrain the final state.
   *
   * @param finalState the final state to constrain to.
   */
  template <typename T>
    requires ScalarLike<T> || MatrixLike<T>
  void ConstrainFinalState(const T& finalState) {
    SubjectTo(FinalState() == finalState);
  }

  /**
   * Set the constraint evaluation function. This function is called
   * `numSteps+1` times, with the corresponding state and input
   * VariableMatrices.
   *
   * @param callback The callback f(t, x, u, dt) where t is time, x is the state
   *   vector, u is the input vector, and dt is the timestep duration.
   */
  void ForEachStep(
      const function_ref<void(const Variable&, const VariableMatrix&,
                              const VariableMatrix&, const Variable&)>
          callback) {
    Variable time = 0.0;

    for (int i = 0; i < m_numSteps + 1; ++i) {
      auto x = X().Col(i);
      auto u = U().Col(i);
      auto dt = DT()(0, i);
      callback(time, x, u, dt);

      time += dt;
    }
  }

  /**
   * Convenience function to set a lower bound on the input.
   *
   * @param lowerBound The lower bound that inputs must always be above. Must be
   * shaped (numInputs)x1.
   */
  template <typename T>
    requires ScalarLike<T> || MatrixLike<T>
  void SetLowerInputBound(const T& lowerBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      SubjectTo(U().Col(i) >= lowerBound);
    }
  }

  /**
   * Convenience function to set an upper bound on the input.
   *
   * @param upperBound The upper bound that inputs must always be below. Must be
   * shaped (numInputs)x1.
   */
  template <typename T>
    requires ScalarLike<T> || MatrixLike<T>
  void SetUpperInputBound(const T& upperBound) {
    for (int i = 0; i < m_numSteps + 1; ++i) {
      SubjectTo(U().Col(i) <= upperBound);
    }
  }

  /**
   * Convenience function to set an upper bound on the timestep.
   *
   * @param maxTimestep The maximum timestep.
   */
  void SetMaxTimestep(std::chrono::duration<double> maxTimestep) {
    SubjectTo(DT() <= maxTimestep.count());
  }

  /**
   * Convenience function to set a lower bound on the timestep.
   *
   * @param minTimestep The minimum timestep.
   */
  void SetMinTimestep(std::chrono::duration<double> minTimestep) {
    SubjectTo(DT() >= minTimestep.count());
  }

  /**
   * Get the state variables. After the problem is solved, this will contain the
   * optimized trajectory.
   *
   * Shaped (numStates)x(numSteps+1).
   *
   * @returns The state variable matrix.
   */
  VariableMatrix& X() { return m_X; };

  /**
   * Get the input variables. After the problem is solved, this will contain the
   * inputs corresponding to the optimized trajectory.
   *
   * Shaped (numInputs)x(numSteps+1), although the last input step is unused in
   * the trajectory.
   *
   * @returns The input variable matrix.
   */
  VariableMatrix& U() { return m_U; };

  /**
   * Get the timestep variables. After the problem is solved, this will contain
   * the timesteps corresponding to the optimized trajectory.
   *
   * Shaped 1x(numSteps+1), although the last timestep is unused in
   * the trajectory.
   *
   * @returns The timestep variable matrix.
   */
  VariableMatrix& DT() { return m_DT; };

  /**
   * Convenience function to get the initial state in the trajectory.
   *
   * @returns The initial state of the trajectory.
   */
  VariableMatrix InitialState() { return m_X.Col(0); }

  /**
   * Convenience function to get the final state in the trajectory.
   *
   * @returns The final state of the trajectory.
   */
  VariableMatrix FinalState() { return m_X.Col(m_numSteps); }

 private:
  void ConstrainDirectCollocation() {
    Assert(m_dynamicsType == DynamicsType::kExplicitODE);

    Variable time = 0.0;

    for (int i = 0; i < m_numSteps; ++i) {
      auto x_begin = X().Col(i);
      auto x_end = X().Col(i + 1);
      auto u_begin = U().Col(i);
      Variable dt = DT()(0, i);
      auto t_begin = time;
      auto t_end = time + dt;
      auto t_c = t_begin + dt / 2.0;

      time += dt;

      // Use u_begin on the end point as well because we are approaching a
      // discontinuity from the left
      auto f_begin = m_dynamicsFunction(t_begin, x_begin, u_begin, dt);
      auto f_end = m_dynamicsFunction(t_end, x_end, u_begin, dt);
      auto x_c = (x_begin + x_end) / 2.0 + (f_begin - f_end) * (dt / 8.0);
      auto xprime_c =
          (x_begin - x_end) * (-3.0 / (2.0 * dt)) - (f_begin + f_end) / 4.0;
      auto f_c = m_dynamicsFunction(t_c, x_c, u_begin, dt);
      SubjectTo(f_c == xprime_c);
    }
  }

  void ConstrainDirectTranscription() {
    Variable time = 0.0;

    for (int i = 0; i < m_numSteps; ++i) {
      auto x_begin = X().Col(i);
      auto x_end = X().Col(i + 1);
      auto u = U().Col(i);
      Variable dt = DT()(0, i);

      if (m_dynamicsType == DynamicsType::kExplicitODE) {
        SubjectTo(x_end ==
                  RK4<const DynamicsFunction&, VariableMatrix, VariableMatrix,
                      Variable>(m_dynamicsFunction, x_begin, u, time, dt));
      } else if (m_dynamicsType == DynamicsType::kDiscrete) {
        SubjectTo(x_end == m_dynamicsFunction(time, x_begin, u, dt));
      }

      time += dt;
    }
  }

  void ConstrainSingleShooting() {
    Variable time = 0.0;

    for (int i = 0; i < m_numSteps; ++i) {
      auto x_begin = X().Col(i);
      auto x_end = X().Col(i + 1);
      auto u = U().Col(i);
      Variable dt = DT()(0, i);

      if (m_dynamicsType == DynamicsType::kExplicitODE) {
        x_end = RK4<const DynamicsFunction&, VariableMatrix, VariableMatrix,
                    Variable>(m_dynamicsFunction, x_begin, u, time, dt);
      } else if (m_dynamicsType == DynamicsType::kDiscrete) {
        x_end = m_dynamicsFunction(time, x_begin, u, dt);
      }

      time += dt;
    }
  }

  int m_numStates;
  int m_numInputs;
  std::chrono::duration<double> m_dt;
  int m_numSteps;
  TranscriptionMethod m_transcriptionMethod;

  DynamicsType m_dynamicsType;
  DynamicsFunction m_dynamicsFunction;

  TimestepMethod m_timestepMethod;

  VariableMatrix m_X;
  VariableMatrix m_U;
  VariableMatrix m_DT;
};

}  // namespace sleipnir
