// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>

#include <fmt/core.h>

#include "frc/trajectory/ExponentialProfile.h"
#include "units/math.h"

namespace frc {
template <class Distance, class Input>
ExponentialProfile<Distance, Input>::ExponentialProfile(Constraints constraints)
    : m_constraints(constraints) {}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::State
ExponentialProfile<Distance, Input>::Calculate(const units::second_t& t,
                                               const State& current,
                                               const State& goal) const {
  auto direction = ShouldFlipInput(current, goal) ? -1 : 1;
  auto u = direction * m_constraints.maxInput;

  auto inflectionPoint = CalculateInflectionPoint(current, goal, u);
  auto timing = CalculateProfileTiming(current, inflectionPoint, goal, u);

  if (t < 0_s) {
    return current;
  } else if (t < timing.inflectionTime) {
    return {ComputeDistanceFromTime(t, u, current),
            ComputeVelocityFromTime(t, u, current)};
  } else if (t < timing.totalTime) {
    return {ComputeDistanceFromTime(t - timing.totalTime, -u, goal),
            ComputeVelocityFromTime(t - timing.totalTime, -u, goal)};
  } else {
    return goal;
  }
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::State
ExponentialProfile<Distance, Input>::CalculateInflectionPoint(
    const State& current, const State& goal) const {
  auto direction = ShouldFlipInput(current, goal) ? -1 : 1;
  auto u = direction * m_constraints.maxInput;

  return CalculateInflectionPoint(current, goal, u);
}

template <class Distance, class Input>
units::second_t ExponentialProfile<Distance, Input>::TimeLeftUntil(
    const State& current, const State& goal) const {
  auto timing = CalculateProfileTiming(current, goal);

  return timing.totalTime;
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::ProfileTiming
ExponentialProfile<Distance, Input>::CalculateProfileTiming(
    const State& current, const State& goal) const {
  auto direction = ShouldFlipInput(current, goal) ? -1 : 1;
  auto u = direction * m_constraints.maxInput;

  auto inflectionPoint = CalculateInflectionPoint(current, goal, u);
  return CalculateProfileTiming(current, inflectionPoint, goal, u);
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::State
ExponentialProfile<Distance, Input>::CalculateInflectionPoint(
    const State& current, const State& goal, const Input_t& input) const {
  auto u = input;

  if (current == goal) {
    return current;
  }

  auto inflectionVelocity = SolveForInflectionVelocity(u, current, goal);
  auto inflectionPosition =
      ComputeDistanceFromVelocity(inflectionVelocity, -u, goal);

  return {inflectionPosition, inflectionVelocity};
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::ProfileTiming
ExponentialProfile<Distance, Input>::CalculateProfileTiming(
    const State& current, const State& inflectionPoint, const State& goal,
    const Input_t& input) const {
  auto u = input;
  auto u_dir = units::math::abs(u) / u;

  units::second_t inflectionT_forward;

  // We need to handle 5 cases here:
  //
  // - Approaching -maxVelocity from below
  // - Approaching -maxVelocity from above
  // - Approaching maxVelocity from below
  // - Approaching maxVelocity from above
  // - At +-maxVelocity
  //
  // For cases 1 and 3, we want to subtract epsilon from the inflection point
  // velocity For cases 2 and 4, we want to add epsilon to the inflection point
  // velocity. For case 5, we have reached inflection point velocity.
  auto epsilon = Velocity_t(1e-9);
  if (units::math::abs(u_dir * m_constraints.MaxVelocity() -
                       inflectionPoint.velocity) < epsilon) {
    auto solvableV = inflectionPoint.velocity;
    units::second_t t_to_solvable_v;
    Distance_t x_at_solvable_v;
    if (units::math::abs(current.velocity - inflectionPoint.velocity) <
        epsilon) {
      t_to_solvable_v = 0_s;
      x_at_solvable_v = current.position;
    } else {
      if (units::math::abs(current.velocity) > m_constraints.MaxVelocity()) {
        solvableV += u_dir * epsilon;
      } else {
        solvableV -= u_dir * epsilon;
      }

      t_to_solvable_v = ComputeTimeFromVelocity(solvableV, u, current.velocity);
      x_at_solvable_v = ComputeDistanceFromVelocity(solvableV, u, current);
    }

    inflectionT_forward =
        t_to_solvable_v + u_dir * (inflectionPoint.position - x_at_solvable_v) /
                              m_constraints.MaxVelocity();
  } else {
    inflectionT_forward =
        ComputeTimeFromVelocity(inflectionPoint.velocity, u, current.velocity);
  }

  auto inflectionT_backward =
      ComputeTimeFromVelocity(inflectionPoint.velocity, -u, goal.velocity);

  return {inflectionT_forward, inflectionT_forward - inflectionT_backward};
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Distance_t
ExponentialProfile<Distance, Input>::ComputeDistanceFromTime(
    const units::second_t& time, const Input_t& input,
    const State& initial) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto u = input;

  return initial.position +
         (-B * u * time +
          (initial.velocity + B * u / A) * (units::math::exp(A * time) - 1)) /
             A;
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Velocity_t
ExponentialProfile<Distance, Input>::ComputeVelocityFromTime(
    const units::second_t& time, const Input_t& input,
    const State& initial) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto u = input;

  return (initial.velocity + B * u / A) * units::math::exp(A * time) -
         B * u / A;
}

template <class Distance, class Input>
units::second_t ExponentialProfile<Distance, Input>::ComputeTimeFromVelocity(
    const Velocity_t& velocity, const Input_t& input,
    const Velocity_t& initial) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto u = input;

  return units::math::log((A * velocity + B * u) / (A * initial + B * u)) / A;
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Distance_t
ExponentialProfile<Distance, Input>::ComputeDistanceFromVelocity(
    const Velocity_t& velocity, const Input_t& input,
    const State& initial) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto u = input;

  return initial.position + (velocity - initial.velocity) / A -
         B * u / (A * A) *
             units::math::log((A * velocity + B * u) /
                              (A * initial.velocity + B * u));
}

template <class Distance, class Input>
typename ExponentialProfile<Distance, Input>::Velocity_t
ExponentialProfile<Distance, Input>::SolveForInflectionVelocity(
    const Input_t& input, const State& current, const State& goal) const {
  auto A = m_constraints.A;
  auto B = m_constraints.B;
  auto u = input;

  auto u_dir = u / units::math::abs(u);

  auto position_delta = goal.position - current.position;
  auto velocity_delta = goal.velocity - current.velocity;

  auto scalar = (A * current.velocity + B * u) * (A * goal.velocity - B * u);
  auto power = -A / B / u * (A * position_delta - velocity_delta);

  auto a = -A * A;
  auto c = B * B * u * u + scalar * units::math::exp(power);

  if (-1e-9 < c.value() && c.value() < 0) {
    // numeric instability - the heuristic gets it right but c is around -1e-13
    return Velocity_t(0);
  }

  return u_dir * units::math::sqrt(-c / a);
}

template <class Distance, class Input>
bool ExponentialProfile<Distance, Input>::ShouldFlipInput(
    const State& current, const State& goal) const {
  auto u = m_constraints.maxInput;

  auto v0 = current.velocity;
  auto xf = goal.position;
  auto vf = goal.velocity;

  auto x_forward = ComputeDistanceFromVelocity(vf, u, current);
  auto x_reverse = ComputeDistanceFromVelocity(vf, -u, current);

  if (v0 >= m_constraints.MaxVelocity()) {
    return xf < x_reverse;
  }

  if (v0 <= -m_constraints.MaxVelocity()) {
    return xf < x_forward;
  }

  auto a = v0 >= Velocity_t(0);
  auto b = vf >= Velocity_t(0);
  auto c = xf >= x_forward;
  auto d = xf >= x_reverse;

  return (a && !d) || (b && !c) || (!c && !d);
}
}  // namespace frc
