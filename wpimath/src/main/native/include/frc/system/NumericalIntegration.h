// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/StateSpaceUtil.h>

#include <algorithm>

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
T RK4(F&& f, T x, units::second_t dt) {
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
T RK4(F&& f, T x, U u, units::second_t dt) {
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

template <typename F, typename T, typename U>
T RKF45Impl(F&& f, T x, U u, double& h, double maxTruncationError,
            double dtRemaining) {
  double truncationErr;
  T newX;

  static double ch[] = {47 / 450.0, 0,        12 / 25.0,
                        32 / 225.0, 1 / 30.0, 6 / 25.0};
  static double ct[] = {-1 / 150.0, 0,         3 / 100.0,
                        -16 / 75.0, -1 / 20.0, 6 / 25.0};
  static Eigen::Matrix<double, 6, 5> Bs = frc::MakeMatrix<6, 5>(
      0.0, 0.0, 0.0, 0.0, 0.0, 2 / 9.0, 0.0, 0.0, 0.0, 0.0, 1 / 12.0, 1 / 4.0,
      0.0, 0.0, 0.0, 69 / 128.0, -243 / 128.0, 135 / 64.0, 0.0, 0.0, -17 / 12.0,
      27 / 4.0, -27 / 5.0, 16 / 15.0, 0.0, 65 / 432.0, -5 / 16.0, 13 / 16.0,
      4 / 27.0, 5 / 144.0);

  do {
    // only allow us to advance up to the dt remaining
    h = std::min(h, dtRemaining);

    // Notice how the derivative in the Wikipedia notation is dy/dx.
    // That means their y is our x and their x is our t
    T k1 = f(x, u) * h;
    T k2 = f(x + (k1 * Bs(1, 0)), u) * h;
    T k3 = f(x + (k1 * (Bs(2, 0))) + (k2 * (Bs(2, 1))), u) * h;
    T k4 =
        f(x + (k1 * (Bs(3, 0))) + (k2 * (Bs(3, 1))) + (k3 * (Bs(3, 2))), u) * h;
    T k5 = f(x + (k1 * (Bs(4, 0))) + (k2 * (Bs(4, 1))) + (k3 * (Bs(4, 2))) +
                 (k4 * (Bs(4, 3))),
             u) *
           h;
    T k6 = f(x + (k1 * (Bs(5, 0))) + (k2 * (Bs(5, 1))) + (k3 * (Bs(5, 2))) +
                 (k4 * (Bs(5, 3))) + (k5 * (Bs(5, 4))),
             u) *
           h;

    newX = x + (k1 * (ch[0])) + (k2 * (ch[1])) + (k3 * (ch[2])) +
           (k4 * (ch[3])) + (k5 * (ch[4])) + (k6 * (ch[5]));

    truncationErr = (k1 * (ct[0]) + (k2 * (ct[1])) + (k3 * (ct[2])) +
                     (k4 * (ct[3])) + (k5 * (ct[4])) + (k6 * (ct[5])))
                        .norm();

    h = 0.9 * h * std::pow(maxTruncationError / truncationErr, 1 / 5.);
  } while (truncationErr > maxTruncationError);

  // Return the new x. Dt is changed by reference
  return newX;
}

/**
 * Performs adaptive RKF45 integration of dx/dt = f(x, u) for dt, as described
 * in https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta%E2%80%93Fehlberg_method
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
T RKF45(F&& f, T x, U u, units::second_t dt, double maxError = 1e-6) {
  double dtElapsed = 0;
  double dtSeconds = dt.to<double>();
  double previousH = dt.to<double>();

  // Loop until we've gotten to our desired dt
  while (dtElapsed < dtSeconds) {
    // RKF45 will give us an updated x and a dt back.
    // We use the new dt (h) as the initial dt for our next loop
    // previousH is changed by-reference
    T ret = RKF45Impl(f, x, u, previousH, maxError, dtSeconds - dtElapsed);
    dtElapsed += previousH;
    x = ret;
  }
  return x;
}

}  // namespace frc
