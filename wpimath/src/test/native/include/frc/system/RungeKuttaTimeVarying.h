// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include "frc/EigenCore.h"
#include "units/time.h"

namespace frc {

/**
 * Performs 4th order Runge-Kutta integration of dy/dt = f(t, y) for dt.
 *
 * @param f  The function to integrate. It must take two arguments t and y.
 * @param t  The initial value of t.
 * @param y  The initial value of y.
 * @param dt The time over which to integrate.
 */
template <typename F, typename T>
T RungeKuttaTimeVarying(F&& f, units::second_t t, T y, units::second_t dt) {
  const auto h = dt.value();

  T k1 = f(t, y);
  T k2 = f(t + dt * 0.5, y + h * k1 * 0.5);
  T k3 = f(t + dt * 0.5, y + h * k2 * 0.5);
  T k4 = f(t + dt, y + h * k3);

  return y + h / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
}

}  // namespace frc
