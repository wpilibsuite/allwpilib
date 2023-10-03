// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>

#include "frc/trajectory/ExponentialProfile.h"
#include "units/math.h"

namespace frc {
template <class Distance, class Input>
ExponentialProfile<Distance, Input>::ExponentialProfile(Constraints constraints,
                                                        State goal,
                                                        State initial)
    : m_constraints(constraints),
      m_initial(initial),
      m_goal(goal),
      m_direction{ShouldFlipInput(constraints, initial, goal) ? -1 : 1} {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto U = m_direction * m_constraints.maxInput;

  if (m_initial == m_goal) {
    m_inflectionTime = 0_s;
    m_totalTime = 0_s;
    m_inflectionPoint = m_initial;

    return;
  }

  auto inflectionV = SolveForInflectionVelocity(U);
  auto inflectionP =
      ComputeDistanceInPhaseSpace(inflectionV, -U, m_goal, m_constraints);

  units::second_t inflectionT_forward;

  auto epsilon = Velocity_t(1e-9);
  if (units::math::abs(m_direction * m_constraints.MaxVelocity() -
                       inflectionV) < epsilon) {
    // A singularity occurrs at A * v_i = B * U
    // we know that we can get _really_ close though and still have stable
    // calculation so we first start by getting really close
    auto solvableV = inflectionV;
    if (units::math::abs(m_initial.velocity) > m_constraints.MaxVelocity()) {
      solvableV += m_direction * epsilon;
    } else {
      solvableV -= m_direction * epsilon;
    }

    auto t_to_solvable_v =
        ComputeTimeFromVelocity(solvableV, U, m_initial.velocity, constraints);
    auto x_at_solvable_v =
        ComputeDistanceInPhaseSpace(solvableV, U, m_initial, m_constraints);

    // and then claim that the remaining distance is covered by the max
    // velocity.
    inflectionT_forward =
        t_to_solvable_v + m_direction * (inflectionP - x_at_solvable_v) /
                              m_constraints.MaxVelocity();
  } else {
    inflectionT_forward = ComputeTimeFromVelocity(
        inflectionV, U, m_initial.velocity, constraints);
  }

  auto inflectionT_backward =
      ComputeTimeFromVelocity(inflectionV, -U, m_goal.velocity, constraints);

  m_inflectionTime = inflectionT_forward;
  m_totalTime = inflectionT_forward - inflectionT_backward;
  m_inflectionPoint = State{inflectionP, inflectionV};
}

template <class Distance, class Input>
bool ExponentialProfile<Distance, Input>::ShouldFlipInput(
    const Constraints& constraints, const State& initial, const State& goal) {
  auto A = constraints.A;
  auto B = constraints.B;
  auto U = constraints.maxInput;

  auto x0 = initial.position;
  auto v0 = initial.velocity;
  auto xf = goal.position;
  auto vf = goal.velocity;

  auto x_forward = ComputeDistanceInPhaseSpace(vf, U, initial, constraints);
  auto x_reverse = ComputeDistanceInPhaseSpace(vf, -U, initial, constraints);

  if (v0 >= -B / A * U) {
    return xf < x_reverse;
  }

  if (v0 <= B / A * U) {
    return xf < x_forward;
  }

  auto a = v0 >= Velocity_t(0);
  auto b = vf >= Velocity_t(0);
  auto c = xf >= x_forward;
  auto d = xf >= x_reverse;

  return (a && !d) || (b && !c) || (!c && !d);
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Distance_t
ExponentialProfile<Distance, Input>::ComputeDistanceInPhaseSpace(
    const Velocity_t& velocity, const Input_t& input, const State& initial,
    const Constraints& constraints) {
  auto A = constraints.A;
  auto B = constraints.B;
  auto U = input;

  return initial.position + (velocity - initial.velocity) / A -
         B * U / (A * A) *
             units::math::log((A * velocity + B * U) /
                              (A * initial.velocity + B * U));
}

template <class Distance, class Input>
units::second_t ExponentialProfile<Distance, Input>::ComputeTimeFromVelocity(
    const Velocity_t& velocity, const Input_t& input, const Velocity_t& initial,
    const Constraints& constraints) {
  auto A = constraints.A;
  auto B = constraints.B;
  auto U = input;

  return units::math::log((A * velocity + B * U) / (A * initial + B * U)) / A;
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Velocity_t
ExponentialProfile<Distance, Input>::SolveForInflectionVelocity(
    const Input_t& input) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto U = input;

  auto U_dir = U / units::math::abs(U);

  auto position_delta = m_goal.position - m_initial.position;
  auto velocity_delta = m_goal.velocity - m_initial.velocity;

  auto scalar =
      (A * m_initial.velocity + B * U) * (A * m_goal.velocity - B * U);
  auto power = -A / B / U * (A * position_delta - velocity_delta);

  auto a = -A * A;
  auto c = B * B * U * U + scalar * units::math::exp(power);

  if (-1e-9 < c.value() && c.value() < 0) {
    // numeric instability - the heuristic gets it right but these are small
    // margins
    return Velocity_t(0);
  }

  return U_dir * units::math::sqrt(-c / a);
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Distance_t
ExponentialProfile<Distance, Input>::DistanceAtTime(
    const units::second_t& time, const Input_t& input) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto U = input;

  if (time < 0_s) {
    return m_initial.position;
  }

  if (time < m_inflectionTime) {
    return m_initial.position +
           (-B * U * time + (m_initial.velocity + B * U / A) *
                                (units::math::exp(A * time) - 1)) /
               A;
  }

  if (time < m_totalTime) {
    return m_goal.position +
           (B * U * (time - m_totalTime) +
            (m_goal.velocity - B * U / A) *
                (units::math::exp(A * (time - m_totalTime)) - 1)) /
               A;
  }

  return m_goal.position;
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Velocity_t
ExponentialProfile<Distance, Input>::VelocityAtTime(
    const units::second_t& time, const Input_t& input) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto U = input;

  if (time < 0_s) {
    return m_initial.velocity;
  }

  if (time < m_inflectionTime) {
    return (m_initial.velocity + B * U / A) * units::math::exp(A * time) -
           B * U / A;
  }

  if (time < m_totalTime) {
    return (m_goal.velocity - B * U / A) *
               units::math::exp(A * (time - m_totalTime)) +
           B * U / A;
  }

  return m_goal.velocity;
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::State
ExponentialProfile<Distance, Input>::Calculate(const units::second_t& t) const {
  return State{DistanceAtTime(t, m_direction * m_constraints.maxInput),
               VelocityAtTime(t, m_direction * m_constraints.maxInput)};
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Input_t
ExponentialProfile<Distance, Input>::CalculateInput(
    const units::second_t& t) const {
  if (t < m_inflectionTime) {
    return m_direction * m_constraints.maxInput;
  } else {
    return -m_direction * m_constraints.maxInput;
  }
}
}  // namespace frc
