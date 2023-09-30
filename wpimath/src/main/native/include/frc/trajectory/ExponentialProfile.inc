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
                                             State goal, State initial)
    : m_constraints(constraints),
      m_initial(initial),
      m_goal(goal),
      m_direction{ShouldFlipInput() ? -1 : 1} {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto U = m_direction * m_constraints.maxInput;

  auto inflectionV = SolveForInflectionVelocity(U);

  auto inflectionP = DistanceForVelocityOnBackwardPass(inflectionV, U);

  auto inflectionT_forward = units::math::log((A * inflectionV + B * U)/(A * m_initial.velocity + B * U)) / A;
  auto inflectionT_backward = units::math::log((A * inflectionV - B * U)/(A * m_goal.velocity - B * U)) / A;

  m_inflectionTime = inflectionT_forward;
  m_totalTime = inflectionT_forward - inflectionT_backward;
  m_inflectionPoint = State{inflectionP, inflectionV};
}

template <class Distance, class Input>
bool
ExponentialProfile<Distance, Input>::ShouldFlipInput() const {
  auto x_forward = DistanceForVelocityOnForwardPass(m_goal.velocity, m_constraints.maxInput);
  auto x_reverse = DistanceForVelocityOnForwardPass(m_goal.velocity, -m_constraints.maxInput);

  auto a = m_initial.velocity >= Velocity_t(0);
  auto b = m_goal.velocity >= Velocity_t(0);
  auto c = m_goal.position >= x_reverse;
  auto d = m_goal.position >= x_forward;

  return (b && c && !d) || (!c && !d) || (a && !c); 
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Distance_t
ExponentialProfile<Distance, Input>::DistanceForVelocityOnForwardPass(const Velocity_t& velocity, const Input_t &input) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto U = input;

  return m_initial.position + (velocity - m_initial.velocity) / A + B * U / (A * A) * units::math::log((A * m_initial.velocity + B * U) / (A * velocity + B * U));
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Distance_t
ExponentialProfile<Distance, Input>::DistanceForVelocityOnBackwardPass(const Velocity_t& velocity, const Input_t &input) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto U = input;

  return m_goal.position + (velocity - m_goal.velocity) / A - B * U / (A * A) * units::math::log((A * m_goal.velocity - B * U) / (A * velocity - B * U));
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Velocity_t
ExponentialProfile<Distance, Input>::SolveForInflectionVelocity(const Input_t &input) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto U = input;

  auto U_dir = units::math::sqrt(U * U) / U;

  auto position_delta = m_goal.position - m_initial.position;
  auto velocity_delta = m_goal.velocity - m_initial.velocity;

  auto A_over_B = m_constraints.A / m_constraints.B;
  auto A_over_B_sq = A_over_B * A_over_B;

  auto scalar = A_over_B_sq * m_initial.velocity * m_goal.velocity + A_over_B * U * velocity_delta - U*U;
  auto power = A_over_B / U * (velocity_delta - A * position_delta);

  auto a = A_over_B_sq;
  auto c = - U*U - scalar * units::math::exp(power);

  return Velocity_t((U_dir * units::math::sqrt(-c) / a).value()); 
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Distance_t 
ExponentialProfile<Distance, Input>::DistanceAtTime(const units::second_t& time, const Input_t& input) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto U = input;

  if (time < 0_s) {
    return m_initial.position;
  }

  if (time < m_inflectionTime) {
    return m_initial.position + (- B * U * time + (m_initial.velocity + B / A * U) * (units::math::exp(A * time) - 1)) / A;
  }

  if (time < m_totalTime) {
    return m_goal.position + (B * U * time + (m_goal.velocity - B / A * U) * (units::math::exp(A * time) - 1)) / A;
  }

  return m_goal.position;
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Velocity_t 
ExponentialProfile<Distance, Input>::VelocityAtTime(const units::second_t& time, const Input_t& input) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto U = input;

  if (time < 0_s) {
    return m_initial.velocity;
  }

  if (time < m_inflectionTime) {
    return (m_initial.velocity + B * U / A) * units::math::exp(A * time) - B * U / A;
  }

  if (time < m_totalTime) {
    return (m_goal.velocity - B * U / A) * units::math::exp(A * time) + B * U / A;
  }

  return m_goal.velocity;
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::State
ExponentialProfile<Distance, Input>::Calculate(const units::second_t &t) const {
  return State{DistanceAtTime(t, m_direction * m_constraints.maxInput), VelocityAtTime(t, m_direction * m_constraints.maxInput)};
}
}  // namespace frc
