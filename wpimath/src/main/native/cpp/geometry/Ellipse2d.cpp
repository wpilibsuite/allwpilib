// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Ellipse2d.hpp"

#include <sleipnir/optimization/problem.hpp>

using namespace frc;

Translation2d Ellipse2d::Nearest(const Translation2d& point) const {
  // Check if already in ellipse
  if (Contains(point)) {
    return point;
  }

  // Rotate the point by the inverse of the ellipse's rotation
  auto rotPoint =
      point.RotateAround(m_center.Translation(), -m_center.Rotation());

  // Find nearest point
  {
    slp::Problem problem;

    // Point on ellipse
    auto x = problem.decision_variable();
    x.set_value(rotPoint.X().value());
    auto y = problem.decision_variable();
    y.set_value(rotPoint.Y().value());

    problem.minimize(slp::pow(x - rotPoint.X().value(), 2) +
                     slp::pow(y - rotPoint.Y().value(), 2));

    // (x − x_c)²/a² + (y − y_c)²/b² = 1
    // b²(x − x_c)² + a²(y − y_c)² = a²b²
    double a2 = m_xSemiAxis.value() * m_xSemiAxis.value();
    double b2 = m_ySemiAxis.value() * m_ySemiAxis.value();
    problem.subject_to(b2 * slp::pow(x - m_center.X().value(), 2) +
                           a2 * slp::pow(y - m_center.Y().value(), 2) ==
                       a2 * b2);

    problem.solve();

    rotPoint = frc::Translation2d{units::meter_t{x.value()},
                                  units::meter_t{y.value()}};
  }

  // Undo rotation
  return rotPoint.RotateAround(m_center.Translation(), m_center.Rotation());
}
