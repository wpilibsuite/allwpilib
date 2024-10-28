// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Ellipse2d.h"

#include <sleipnir/optimization/OptimizationProblem.hpp>

using namespace frc;

Translation2d Ellipse2d::FindNearestPoint(const Translation2d& point) const {
  // Check if already in ellipse
  if (Contains(point)) {
    return point;
  }

  // Rotate the point by the inverse of the ellipse's rotation
  auto rotPoint =
      point.RotateAround(m_center.Translation(), -m_center.Rotation());

  // Find nearest point
  {
    namespace slp = sleipnir;

    sleipnir::OptimizationProblem problem;

    // Point on ellipse
    auto x = problem.DecisionVariable();
    x.SetValue(rotPoint.X().value());
    auto y = problem.DecisionVariable();
    y.SetValue(rotPoint.Y().value());

    problem.Minimize(slp::pow(x - rotPoint.X().value(), 2) +
                     slp::pow(y - rotPoint.Y().value(), 2));

    // (x − x_c)²/a² + (y − y_c)²/b² = 1
    problem.SubjectTo(slp::pow(x - m_center.X().value(), 2) /
                              (m_xSemiAxis.value() * m_xSemiAxis.value()) +
                          slp::pow(y - m_center.Y().value(), 2) /
                              (m_ySemiAxis.value() * m_ySemiAxis.value()) ==
                      1);

    problem.Solve();

    rotPoint = frc::Translation2d{units::meter_t{x.Value()},
                                  units::meter_t{y.Value()}};
  }

  // Undo rotation
  return rotPoint.RotateAround(m_center.Translation(), m_center.Rotation());
}
