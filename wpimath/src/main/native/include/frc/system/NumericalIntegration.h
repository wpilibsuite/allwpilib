// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <array>
#include <cmath>

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
T RK4(F&& f, T x, units::second_t dt) {
  const auto h = dt.value();

  T k1 = f(x);
  T k2 = f(x + h * 0.5 * k1);
  T k3 = f(x + h * 0.5 * k2);
  T k4 = f(x + h * k3);

  return x + h / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
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
T RK4(F&& f, T x, U u, units::second_t dt) {
  const auto h = dt.value();

  T k1 = f(x, u);
  T k2 = f(x + h * 0.5 * k1, u);
  T k3 = f(x + h * 0.5 * k2, u);
  T k4 = f(x + h * k3, u);

  return x + h / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
}

/**
 * Performs adaptive Dormand-Prince integration of dx/dt = f(x, u) for dt.
 *
 * @param f        The function to integrate. It must take two arguments x and
 *                 u.
 * @param x        The initial value of x.
 * @param u        The value u held constant over the integration period.
 * @param dt       The time over which to integrate.
 * @param maxError The maximum acceptable truncation error. Usually a small
 *                 number like 1e-6.
 */
template <typename F, typename T, typename U>
T RKDP(F&& f, T x, U u, units::second_t dt, double maxError = 1e-6) {
  // See https://en.wikipedia.org/wiki/Dormand%E2%80%93Prince_method for the
  // Butcher tableau the following arrays came from.

  constexpr int kDim = 7;

  // clang-format off
  constexpr double A[kDim - 1][kDim - 1]{
      {      1.0 / 5.0},
      {      3.0 / 40.0,        9.0 / 40.0},
      {     44.0 / 45.0,      -56.0 / 15.0,       32.0 / 9.0},
      {19372.0 / 6561.0, -25360.0 / 2187.0, 64448.0 / 6561.0, -212.0 / 729.0},
      { 9017.0 / 3168.0,     -355.0 / 33.0, 46732.0 / 5247.0,   49.0 / 176.0, -5103.0 / 18656.0},
      {    35.0 / 384.0,               0.0,   500.0 / 1113.0,  125.0 / 192.0,  -2187.0 / 6784.0, 11.0 / 84.0}};
  // clang-format on

  constexpr std::array<double, kDim> b1{
      35.0 / 384.0, 0.0, 500.0 / 1113.0, 125.0 / 192.0, -2187.0 / 6784.0,
      11.0 / 84.0,  0.0};
  constexpr std::array<double, kDim> b2{5179.0 / 57600.0,    0.0,
                                        7571.0 / 16695.0,    393.0 / 640.0,
                                        -92097.0 / 339200.0, 187.0 / 2100.0,
                                        1.0 / 40.0};

  T newX;
  double truncationError;

  double dtElapsed = 0.0;
  double h = dt.value();

  // Loop until we've gotten to our desired dt
  while (dtElapsed < dt.value()) {
    do {
      // Only allow us to advance up to the dt remaining
      h = (std::min)(h, dt.value() - dtElapsed);

      // clang-format off
      T k1 = f(x, u);
      T k2 = f(x + h * (A[0][0] * k1), u);
      T k3 = f(x + h * (A[1][0] * k1 + A[1][1] * k2), u);
      T k4 = f(x + h * (A[2][0] * k1 + A[2][1] * k2 + A[2][2] * k3), u);
      T k5 = f(x + h * (A[3][0] * k1 + A[3][1] * k2 + A[3][2] * k3 + A[3][3] * k4), u);
      T k6 = f(x + h * (A[4][0] * k1 + A[4][1] * k2 + A[4][2] * k3 + A[4][3] * k4 + A[4][4] * k5), u);
      // clang-format on

      // Since the final row of A and the array b1 have the same coefficients
      // and k7 has no effect on newX, we can reuse the calculation.
      newX = x + h * (A[5][0] * k1 + A[5][1] * k2 + A[5][2] * k3 +
                      A[5][3] * k4 + A[5][4] * k5 + A[5][5] * k6);
      T k7 = f(newX, u);

      truncationError = (h * ((b1[0] - b2[0]) * k1 + (b1[1] - b2[1]) * k2 +
                              (b1[2] - b2[2]) * k3 + (b1[3] - b2[3]) * k4 +
                              (b1[4] - b2[4]) * k5 + (b1[5] - b2[5]) * k6 +
                              (b1[6] - b2[6]) * k7))
                            .norm();

      if (truncationError == 0.0) {
        h = dt.value() - dtElapsed;
      } else {
        h *= 0.9 * std::pow(maxError / truncationError, 1.0 / 5.0);
      }
    } while (truncationError > maxError);

    dtElapsed += h;
    x = newX;
  }

  return x;
}

}  // namespace frc
