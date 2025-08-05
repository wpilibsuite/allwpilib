// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

#include <chrono>
#include <utility>

#include "sleipnir/autodiff/variable_matrix.hpp"
#include "sleipnir/optimization/problem.hpp"
#include "sleipnir/util/assert.hpp"
#include "sleipnir/util/concepts.hpp"
#include "sleipnir/util/function_ref.hpp"
#include "sleipnir/util/symbol_exports.hpp"

namespace slp {

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
State rk4(F&& f, State x, Input u, Time t0, Time dt) {
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
  DIRECT_TRANSCRIPTION,
  /// The trajectory is modeled as a series of cubic polynomials where the
  /// centerpoint slope is constrained.
  DIRECT_COLLOCATION,
  /// States depend explicitly as a function of all previous states and all
  /// previous inputs.
  SINGLE_SHOOTING
};

/**
 * Enum describing a type of system dynamics constraints.
 */
enum class DynamicsType : uint8_t {
  /// The dynamics are a function in the form dx/dt = f(t, x, u).
  EXPLICIT_ODE,
  /// The dynamics are a function in the form xₖ₊₁ = f(t, xₖ, uₖ).
  DISCRETE
};

/**
 * Enum describing the type of system timestep.
 */
enum class TimestepMethod : uint8_t {
  /// The timestep is a fixed constant.
  FIXED,
  /// The timesteps are allowed to vary as independent decision variables.
  VARIABLE,
  /// The timesteps are equal length but allowed to vary as a single decision
  /// variable.
  VARIABLE_SINGLE
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
class SLEIPNIR_DLLEXPORT OCP : public Problem {
 public:
  /**
   * Build an optimization problem using a system evolution function (explicit
   * ODE or discrete state transition function).
   *
   * @param num_states The number of system states.
   * @param num_inputs The number of system inputs.
   * @param dt The timestep for fixed-step integration.
   * @param num_steps The number of control points.
   * @param dynamics Function representing an explicit or implicit ODE, or a
   *   discrete state transition function.
   *   - Explicit: dx/dt = f(x, u, *)
   *   - Implicit: f([x dx/dt]', u, *) = 0
   *   - State transition: xₖ₊₁ = f(xₖ, uₖ)
   * @param dynamics_type The type of system evolution function.
   * @param timestep_method The timestep method.
   * @param method The transcription method.
   */
  OCP(int num_states, int num_inputs, std::chrono::duration<double> dt,
      int num_steps,
      function_ref<VariableMatrix(const VariableMatrix& x,
                                  const VariableMatrix& u)>
          dynamics,
      DynamicsType dynamics_type = DynamicsType::EXPLICIT_ODE,
      TimestepMethod timestep_method = TimestepMethod::FIXED,
      TranscriptionMethod method = TranscriptionMethod::DIRECT_TRANSCRIPTION)
      : OCP{num_states,
            num_inputs,
            dt,
            num_steps,
            [=]([[maybe_unused]] const VariableMatrix& t,
                const VariableMatrix& x, const VariableMatrix& u,
                [[maybe_unused]]
                const VariableMatrix& dt) -> VariableMatrix {
              return dynamics(x, u);
            },
            dynamics_type,
            timestep_method,
            method} {}

  /**
   * Build an optimization problem using a system evolution function (explicit
   * ODE or discrete state transition function).
   *
   * @param num_states The number of system states.
   * @param num_inputs The number of system inputs.
   * @param dt The timestep for fixed-step integration.
   * @param num_steps The number of control points.
   * @param dynamics Function representing an explicit or implicit ODE, or a
   *   discrete state transition function.
   *   - Explicit: dx/dt = f(t, x, u, *)
   *   - Implicit: f(t, [x dx/dt]', u, *) = 0
   *   - State transition: xₖ₊₁ = f(t, xₖ, uₖ, dt)
   * @param dynamics_type The type of system evolution function.
   * @param timestep_method The timestep method.
   * @param method The transcription method.
   */
  OCP(int num_states, int num_inputs, std::chrono::duration<double> dt,
      int num_steps,
      function_ref<VariableMatrix(const Variable& t, const VariableMatrix& x,
                                  const VariableMatrix& u, const Variable& dt)>
          dynamics,
      DynamicsType dynamics_type = DynamicsType::EXPLICIT_ODE,
      TimestepMethod timestep_method = TimestepMethod::FIXED,
      TranscriptionMethod method = TranscriptionMethod::DIRECT_TRANSCRIPTION)
      : m_num_states{num_states},
        m_num_inputs{num_inputs},
        m_dt{dt},
        m_num_steps{num_steps},
        m_transcription_method{method},
        m_dynamics_type{dynamics_type},
        m_dynamics_function{std::move(dynamics)},
        m_timestep_method{timestep_method} {
    // u is num_steps + 1 so that the final constraint function evaluation works
    m_U = decision_variable(m_num_inputs, m_num_steps + 1);

    if (m_timestep_method == TimestepMethod::FIXED) {
      m_DT = VariableMatrix{1, m_num_steps + 1};
      for (int i = 0; i < num_steps + 1; ++i) {
        m_DT(0, i) = m_dt.count();
      }
    } else if (m_timestep_method == TimestepMethod::VARIABLE_SINGLE) {
      Variable dt = decision_variable();
      dt.set_value(m_dt.count());

      // Set the member variable matrix to track the decision variable
      m_DT = VariableMatrix{1, m_num_steps + 1};
      for (int i = 0; i < num_steps + 1; ++i) {
        m_DT(0, i) = dt;
      }
    } else if (m_timestep_method == TimestepMethod::VARIABLE) {
      m_DT = decision_variable(1, m_num_steps + 1);
      for (int i = 0; i < num_steps + 1; ++i) {
        m_DT(0, i).set_value(m_dt.count());
      }
    }

    if (m_transcription_method == TranscriptionMethod::DIRECT_TRANSCRIPTION) {
      m_X = decision_variable(m_num_states, m_num_steps + 1);
      constrain_direct_transcription();
    } else if (m_transcription_method ==
               TranscriptionMethod::DIRECT_COLLOCATION) {
      m_X = decision_variable(m_num_states, m_num_steps + 1);
      constrain_direct_collocation();
    } else if (m_transcription_method == TranscriptionMethod::SINGLE_SHOOTING) {
      // In single-shooting the states aren't decision variables, but instead
      // depend on the input and previous states
      m_X = VariableMatrix{m_num_states, m_num_steps + 1};
      constrain_single_shooting();
    }
  }

  /**
   * Utility function to constrain the initial state.
   *
   * @param initial_state the initial state to constrain to.
   */
  template <typename T>
    requires ScalarLike<T> || MatrixLike<T>
  void constrain_initial_state(const T& initial_state) {
    subject_to(this->initial_state() == initial_state);
  }

  /**
   * Utility function to constrain the final state.
   *
   * @param final_state the final state to constrain to.
   */
  template <typename T>
    requires ScalarLike<T> || MatrixLike<T>
  void constrain_final_state(const T& final_state) {
    subject_to(this->final_state() == final_state);
  }

  /**
   * Set the constraint evaluation function. This function is called
   * `num_steps+1` times, with the corresponding state and input
   * VariableMatrices.
   *
   * @param callback The callback f(x, u) where x is the state and u is the
   *     input vector.
   */
  void for_each_step(
      const function_ref<void(const VariableMatrix& x, const VariableMatrix& u)>
          callback) {
    for (int i = 0; i < m_num_steps + 1; ++i) {
      auto x = X().col(i);
      auto u = U().col(i);
      callback(x, u);
    }
  }

  /**
   * Set the constraint evaluation function. This function is called
   * `num_steps+1` times, with the corresponding state and input
   * VariableMatrices.
   *
   * @param callback The callback f(t, x, u, dt) where t is time, x is the state
   *   vector, u is the input vector, and dt is the timestep duration.
   */
  void for_each_step(
      const function_ref<void(const Variable& t, const VariableMatrix& x,
                              const VariableMatrix& u, const Variable& dt)>
          callback) {
    Variable time = 0.0;

    for (int i = 0; i < m_num_steps + 1; ++i) {
      auto x = X().col(i);
      auto u = U().col(i);
      auto dt = this->dt()(0, i);
      callback(time, x, u, dt);

      time += dt;
    }
  }

  /**
   * Convenience function to set a lower bound on the input.
   *
   * @param lower_bound The lower bound that inputs must always be above. Must
   *   be shaped (num_inputs)x1.
   */
  template <typename T>
    requires ScalarLike<T> || MatrixLike<T>
  void set_lower_input_bound(const T& lower_bound) {
    for (int i = 0; i < m_num_steps + 1; ++i) {
      subject_to(U().col(i) >= lower_bound);
    }
  }

  /**
   * Convenience function to set an upper bound on the input.
   *
   * @param upper_bound The upper bound that inputs must always be below. Must
   *   be shaped (num_inputs)x1.
   */
  template <typename T>
    requires ScalarLike<T> || MatrixLike<T>
  void set_upper_input_bound(const T& upper_bound) {
    for (int i = 0; i < m_num_steps + 1; ++i) {
      subject_to(U().col(i) <= upper_bound);
    }
  }

  /**
   * Convenience function to set a lower bound on the timestep.
   *
   * @param min_timestep The minimum timestep.
   */
  void set_min_timestep(std::chrono::duration<double> min_timestep) {
    subject_to(dt() >= min_timestep.count());
  }

  /**
   * Convenience function to set an upper bound on the timestep.
   *
   * @param max_timestep The maximum timestep.
   */
  void set_max_timestep(std::chrono::duration<double> max_timestep) {
    subject_to(dt() <= max_timestep.count());
  }

  /**
   * Get the state variables. After the problem is solved, this will contain the
   * optimized trajectory.
   *
   * Shaped (num_states)x(num_steps+1).
   *
   * @return The state variable matrix.
   */
  VariableMatrix& X() { return m_X; }

  /**
   * Get the input variables. After the problem is solved, this will contain the
   * inputs corresponding to the optimized trajectory.
   *
   * Shaped (num_inputs)x(num_steps+1), although the last input step is unused
   * in the trajectory.
   *
   * @return The input variable matrix.
   */
  VariableMatrix& U() { return m_U; }

  /**
   * Get the timestep variables. After the problem is solved, this will contain
   * the timesteps corresponding to the optimized trajectory.
   *
   * Shaped 1x(num_steps+1), although the last timestep is unused in
   * the trajectory.
   *
   * @return The timestep variable matrix.
   */
  VariableMatrix& dt() { return m_DT; }

  /**
   * Convenience function to get the initial state in the trajectory.
   *
   * @return The initial state of the trajectory.
   */
  VariableMatrix initial_state() { return m_X.col(0); }

  /**
   * Convenience function to get the final state in the trajectory.
   *
   * @return The final state of the trajectory.
   */
  VariableMatrix final_state() { return m_X.col(m_num_steps); }

 private:
  void constrain_direct_collocation() {
    slp_assert(m_dynamics_type == DynamicsType::EXPLICIT_ODE);

    Variable time = 0.0;

    // Derivation at https://mec560sbu.github.io/2016/09/30/direct_collocation/
    for (int i = 0; i < m_num_steps; ++i) {
      Variable h = dt()(0, i);

      auto& f = m_dynamics_function;

      auto t_begin = time;
      auto t_end = t_begin + h;

      auto x_begin = X().col(i);
      auto x_end = X().col(i + 1);

      auto u_begin = U().col(i);
      auto u_end = U().col(i + 1);

      auto xdot_begin = f(t_begin, x_begin, u_begin, h);
      auto xdot_end = f(t_end, x_end, u_end, h);
      auto xdot_c =
          -3 / (2 * h) * (x_begin - x_end) - 0.25 * (xdot_begin + xdot_end);

      auto t_c = t_begin + 0.5 * h;
      auto x_c = 0.5 * (x_begin + x_end) + h / 8 * (xdot_begin - xdot_end);
      auto u_c = 0.5 * (u_begin + u_end);

      subject_to(xdot_c == f(t_c, x_c, u_c, h));

      time += h;
    }
  }

  void constrain_direct_transcription() {
    Variable time = 0.0;

    for (int i = 0; i < m_num_steps; ++i) {
      auto x_begin = X().col(i);
      auto x_end = X().col(i + 1);
      auto u = U().col(i);
      Variable dt = this->dt()(0, i);

      if (m_dynamics_type == DynamicsType::EXPLICIT_ODE) {
        subject_to(x_end == rk4<const decltype(m_dynamics_function)&,
                                VariableMatrix, VariableMatrix, Variable>(
                                m_dynamics_function, x_begin, u, time, dt));
      } else if (m_dynamics_type == DynamicsType::DISCRETE) {
        subject_to(x_end == m_dynamics_function(time, x_begin, u, dt));
      }

      time += dt;
    }
  }

  void constrain_single_shooting() {
    Variable time = 0.0;

    for (int i = 0; i < m_num_steps; ++i) {
      auto x_begin = X().col(i);
      auto x_end = X().col(i + 1);
      auto u = U().col(i);
      Variable dt = this->dt()(0, i);

      if (m_dynamics_type == DynamicsType::EXPLICIT_ODE) {
        x_end = rk4<const decltype(m_dynamics_function)&, VariableMatrix,
                    VariableMatrix, Variable>(m_dynamics_function, x_begin, u,
                                              time, dt);
      } else if (m_dynamics_type == DynamicsType::DISCRETE) {
        x_end = m_dynamics_function(time, x_begin, u, dt);
      }

      time += dt;
    }
  }

  int m_num_states;
  int m_num_inputs;
  std::chrono::duration<double> m_dt;
  int m_num_steps;
  TranscriptionMethod m_transcription_method;

  DynamicsType m_dynamics_type;

  function_ref<VariableMatrix(const Variable& t, const VariableMatrix& x,
                              const VariableMatrix& u, const Variable& dt)>
      m_dynamics_function;

  TimestepMethod m_timestep_method;

  VariableMatrix m_X;
  VariableMatrix m_U;
  VariableMatrix m_DT;
};

}  // namespace slp
