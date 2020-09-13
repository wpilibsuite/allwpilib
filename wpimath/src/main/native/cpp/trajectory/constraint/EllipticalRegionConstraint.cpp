/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/constraint/EllipticalRegionConstraint.h"

#include <limits>

#include "units/math.h"

using namespace frc;

EllipticalRegionConstraint::EllipticalRegionConstraint(
    const Translation2d& center, units::meter_t xWidth, units::meter_t yWidth,
    const Rotation2d& rotation, const TrajectoryConstraint& constraint)
    : m_center(center),
      m_radii(xWidth / 2.0, yWidth / 2.0),
      m_constraint(constraint) {
  m_radii = m_radii.RotateBy(rotation);
}

units::meters_per_second_t EllipticalRegionConstraint::MaxVelocity(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t velocity) const {
  if (IsPoseInRegion(pose)) {
    return m_constraint.MaxVelocity(pose, curvature, velocity);
  } else {
    return units::meters_per_second_t(std::numeric_limits<double>::infinity());
  }
}

TrajectoryConstraint::MinMax EllipticalRegionConstraint::MinMaxAcceleration(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t speed) const {
  if (IsPoseInRegion(pose)) {
    return m_constraint.MinMaxAcceleration(pose, curvature, speed);
  } else {
    return {};
  }
}

bool EllipticalRegionConstraint::IsPoseInRegion(const Pose2d& pose) const {
  // The region (disk) bounded by the ellipse is given by the equation:
  // ((x-h)^2)/Rx^2) + ((y-k)^2)/Ry^2) <= 1
  // If the inequality is satisfied, then it is inside the ellipse; otherwise
  // it is outside the ellipse.
  // Both sides have been multiplied by Rx^2 * Ry^2 for efficiency reasons.
  return units::math::pow<2>(pose.X() - m_center.X()) *
                 units::math::pow<2>(m_radii.Y()) +
             units::math::pow<2>(pose.Y() - m_center.Y()) *
                 units::math::pow<2>(m_radii.X()) <=
         units::math::pow<2>(m_radii.X()) * units::math::pow<2>(m_radii.Y());
}
