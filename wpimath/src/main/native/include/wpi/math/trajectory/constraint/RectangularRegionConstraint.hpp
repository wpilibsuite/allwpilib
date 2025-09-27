// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <limits>

#include "frc/geometry/Rectangle2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"

namespace frc {

/**
 * Enforces a particular constraint only within a rectangular region.
 */
template <std::derived_from<TrajectoryConstraint> Constraint>
class RectangularRegionConstraint : public TrajectoryConstraint {
 public:
  /**
   * Constructs a new RectangularRegionConstraint.
   *
   * @param bottomLeftPoint The bottom left point of the rectangular region in
   *     which to enforce the constraint.
   * @param topRightPoint The top right point of the rectangular region in which
   *     to enforce the constraint.
   * @param constraint The constraint to enforce when the robot is within the
   *     region.
   * @deprecated Use constructor taking Rectangle2d instead.
   */
  [[deprecated("Use constructor taking Rectangle2d instead.")]]
  constexpr RectangularRegionConstraint(const Translation2d& bottomLeftPoint,
                                        const Translation2d& topRightPoint,
                                        const Constraint& constraint)
      : m_rectangle{bottomLeftPoint, topRightPoint}, m_constraint(constraint) {}

  /**
   * Constructs a new RectangularRegionConstraint.
   *
   * @param rectangle The rectangular region in which to enforce the constraint.
   * @param constraint The constraint to enforce when the robot is within the
   *     region.
   */
  constexpr RectangularRegionConstraint(const Rectangle2d& rectangle,
                                        const Constraint& constraint)
      : m_rectangle{rectangle}, m_constraint{constraint} {}

  constexpr units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) const override {
    if (m_rectangle.Contains(pose.Translation())) {
      return m_constraint.MaxVelocity(pose, curvature, velocity);
    } else {
      return units::meters_per_second_t{
          std::numeric_limits<double>::infinity()};
    }
  }

  constexpr MinMax MinMaxAcceleration(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t speed) const override {
    if (m_rectangle.Contains(pose.Translation())) {
      return m_constraint.MinMaxAcceleration(pose, curvature, speed);
    } else {
      return {};
    }
  }

 private:
  Rectangle2d m_rectangle;
  Constraint m_constraint;
};

}  // namespace frc
