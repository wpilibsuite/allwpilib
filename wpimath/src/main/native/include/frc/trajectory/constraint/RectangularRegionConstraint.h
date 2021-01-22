// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <limits>

#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"

namespace frc {
/**
 * Enforces a particular constraint only within a rectangular region.
 */
template <typename Constraint, typename = std::enable_if_t<std::is_base_of_v<
                                   TrajectoryConstraint, Constraint>>>
class RectangularRegionConstraint : public TrajectoryConstraint {
 public:
  /**
   * Constructs a new RectangularRegionConstraint.
   *
   * @param bottomLeftPoint The bottom left point of the rectangular region in
   * which to enforce the constraint.
   * @param topRightPoint The top right point of the rectangular region in which
   * to enforce the constraint.
   * @param constraint The constraint to enforce when the robot is within the
   * region.
   */
  RectangularRegionConstraint(const Translation2d& bottomLeftPoint,
                              const Translation2d& topRightPoint,
                              const Constraint& constraint)
      : m_bottomLeftPoint(bottomLeftPoint),
        m_topRightPoint(topRightPoint),
        m_constraint(constraint) {}

  units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) const override {
    if (IsPoseInRegion(pose)) {
      return m_constraint.MaxVelocity(pose, curvature, velocity);
    } else {
      return units::meters_per_second_t(
          std::numeric_limits<double>::infinity());
    }
  }

  MinMax MinMaxAcceleration(const Pose2d& pose, units::curvature_t curvature,
                            units::meters_per_second_t speed) const override {
    if (IsPoseInRegion(pose)) {
      return m_constraint.MinMaxAcceleration(pose, curvature, speed);
    } else {
      return {};
    }
  }

  /**
   * Returns whether the specified robot pose is within the region that the
   * constraint is enforced in.
   *
   * @param pose The robot pose.
   * @return Whether the robot pose is within the constraint region.
   */
  bool IsPoseInRegion(const Pose2d& pose) const {
    return pose.X() >= m_bottomLeftPoint.X() &&
           pose.X() <= m_topRightPoint.X() &&
           pose.Y() >= m_bottomLeftPoint.Y() && pose.Y() <= m_topRightPoint.Y();
  }

 private:
  Translation2d m_bottomLeftPoint;
  Translation2d m_topRightPoint;
  Constraint m_constraint;
};
}  // namespace frc
