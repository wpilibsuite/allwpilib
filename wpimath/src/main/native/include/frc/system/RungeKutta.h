// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Eigen/Core"
#include "units/time.h"

namespace frc {

/**
 * Performs 4th order Runge-Kutta integration of dx/dt = f(x) for dt.
 *
 * @param f  The function to integrate. It must take one argument x.
 * @param x  The initial value of x.
 * @param dt The time over which to integrate.
 */
template <typename F, typename T>
T RungeKutta(F&& f, T x, units::second_t dt) {
  const auto halfDt = 0.5 * dt;
  T k1 = f(x);
  T k2 = f(x + k1 * halfDt.to<double>());
  T k3 = f(x + k2 * halfDt.to<double>());
  T k4 = f(x + k3 * dt.to<double>());
  return x + dt.to<double>() / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
}

/**
 * Performs 4th order Runge-Kutta integration of dx/dt = f(x, u) for dt.
 *
 * @param f  The function to integrate. It must take two arguments x and u.
 * @param x  The initial value of x.
 * @param u  The value u held constant over the integration period.
 * @param dt The time over which to integrate.
 */
template <typename F, typename T, typename U>
T RungeKutta(F&& f, T x, U u, units::second_t dt) {
  const auto halfDt = 0.5 * dt;
  T k1 = f(x, u);
  T k2 = f(x + k1 * halfDt.to<double>(), u);
  T k3 = f(x + k2 * halfDt.to<double>(), u);
  T k4 = f(x + k3 * dt.to<double>(), u);
  return x + dt.to<double>() / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
}

/**
 * Performs 4th order Runge-Kutta integration of dx/dt = f(t, x) for dt.
 *
 * @param f  The function to integrate. It must take two arguments x and t.
 * @param x  The initial value of x.
 * @param t  The initial value of t.
 * @param dt The time over which to integrate.
 */
template <typename F, typename T>
T RungeKuttaTimeVarying(F&& f, T x, units::second_t t, units::second_t dt) {
  const auto halfDt = 0.5 * dt;
  T k1 = f(t, x);
  T k2 = f(t + halfDt, x + k1 / 2.0);
  T k3 = f(t + halfDt, x + k2 / 2.0);
  T k4 = f(t + dt, x + k3);

  return x + dt.to<double>() / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
}

}  // namespace frc
