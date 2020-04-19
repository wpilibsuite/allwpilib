/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/constraint/RectangularRegionConstraint.h"

#include <limits>

using namespace frc;

RectangularRegionConstraint::RectangularRegionConstraint(
    const Translation2d& bottomLeftPoint, const Translation2d& topRightPoint,
    const TrajectoryConstraint& constraint)
    : m_bottomLeftPoint(bottomLeftPoint),
      m_topRightPoint(topRightPoint),
      m_constraint(constraint) {}

units::meters_per_second_t RectangularRegionConstraint::MaxVelocity(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t velocity) const {
  if (IsPoseInRegion(pose)) {
    return m_constraint.MaxVelocity(pose, curvature, velocity);
  } else {
    return units::meters_per_second_t(std::numeric_limits<double>::infinity());
  }
}

TrajectoryConstraint::MinMax RectangularRegionConstraint::MinMaxAcceleration(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t speed) const {
  if (IsPoseInRegion(pose)) {
    return m_constraint.MinMaxAcceleration(pose, curvature, speed);
  } else {
    return {};
  }
}

bool RectangularRegionConstraint::IsPoseInRegion(const Pose2d& pose) const {
  return pose.Translation().X() >= m_bottomLeftPoint.X() &&
         pose.Translation().X() <= m_topRightPoint.X() &&
         pose.Translation().Y() >= m_bottomLeftPoint.Y() &&
         pose.Translation().Y() <= m_topRightPoint.Y();
}
