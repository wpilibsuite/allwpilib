/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <utility>
#include <vector>

#include <Eigen/Core>

#include "frc/geometry/Pose2d.h"

namespace frc {

/**
 * Define a unit for curvature.
 */
using curvature_t = units::unit_t<
    units::compound_unit<units::radian, units::inverse<units::meter>>>;

/**
 * Represents a two-dimensional parametric spline that interpolates between two
 * points.
 *
 * @tparam Degree The degree of the spline.
 */
template <int Degree>
class Spline {
 public:
  using PoseWithCurvature = std::pair<Pose2d, curvature_t>;

  /**
   * Parameterizes the spline. This method breaks up the spline into various
   * arcs until their dx, dy, and dtheta are within specific tolerances.
   *
   * This parameterization technique was derived by Team 254. The original
   * source is here:
   * <https://github.com/Team254/FRC-2018-Public/blob/master/src/main/java/com/team254/lib/spline/SplineGenerator.java#L48>
   *
   * @param t0 Starting internal spline parameter. It is recommended to leave
   * this as default.
   * @param t1 Ending internal spline parameter. It is recommended to leave this
   * as default.
   *
   * @return A vector of poses and curvatures that represents various points on
   * the spline.
   */
  std::vector<PoseWithCurvature> Parameterize(double t0 = 0.0,
                                              double t1 = 1.0) const {
    std::vector<PoseWithCurvature> arr;

    // The parameterization does not add the first initial point. Let's add
    // that.
    arr.push_back(GetPoint(t0));

    GetSegmentArc(&arr, t0, t1);
    return arr;
  }

  /**
   * Gets the pose and curvature at some point t on the spline.
   *
   * @param t The point t
   * @return The pose and curvature at that point.
   */
  PoseWithCurvature GetPoint(double t) const {
    Eigen::Matrix<double, Degree + 1, 1> polynomialBases;

    // Populate the polynomial bases
    for (int i = 0; i <= Degree; i++) {
      polynomialBases(i) = std::pow(t, Degree - i);
    }

    // This simply multiplies by the coefficients. We need to divide out t some
    // n number of times where n is the derivative we want to take.
    Eigen::Matrix<double, 6, 1> combined = Coefficients() * polynomialBases;

    double dx, dy, ddx, ddy;

    // If t = 0, all other terms in the equation cancel out to zero. We can use
    // the last x^0 term in the equation.
    if (t == 0.0) {
      dx = Coefficients()(2, Degree - 1);
      dy = Coefficients()(3, Degree - 1);
      ddx = Coefficients()(4, Degree - 2);
      ddy = Coefficients()(5, Degree - 2);
    } else {
      // Divide out t for first derivative.
      dx = combined(2) / t;
      dy = combined(3) / t;

      // Divide out t for second derivative.
      ddx = combined(4) / t / t;
      ddy = combined(5) / t / t;
    }

    // Find the curvature.
    const auto curvature =
        (dx * ddy - ddx * dy) / ((dx * dx + dy * dy) * std::hypot(dx, dy));

    return {
        {FromVector(combined.template block<2, 1>(0, 0)), Rotation2d(dx, dy)},
        curvature_t(curvature)};
  }

 protected:
  /**
   * Returns the coefficients of the spline.
   *
   * @return The coefficients of the spline.
   */
  virtual Eigen::Matrix<double, 6, Degree + 1> Coefficients() const = 0;

  /**
   * Converts a Translation2d into a vector that is compatible with Eigen.
   *
   * @param translation The Translation2d to convert.
   * @return The vector.
   */
  static Eigen::Vector2d ToVector(const Translation2d& translation) {
    return (Eigen::Vector2d() << translation.X().to<double>(),
            translation.Y().to<double>())
        .finished();
  }

  /**
   * Converts an Eigen vector into a Translation2d.
   *
   * @param vector The vector to convert.
   * @return The Translation2d.
   */
  static Translation2d FromVector(const Eigen::Vector2d& vector) {
    return Translation2d(units::meter_t(vector(0)), units::meter_t(vector(1)));
  }

 private:
  // Constraints for spline parameterization.
  constexpr static units::meter_t kMaxDx = 5_in;
  constexpr static units::meter_t kMaxDy = 0.05_in;
  constexpr static units::radian_t kMaxDtheta = 0.0872_rad;

  /**
   * Breaks up the spline into arcs until the dx, dy, and theta of each arc is
   * within tolerance.
   *
   * @param vector Pointer to vector of poses.
   * @param t0 Starting point for arc.
   * @param t1 Ending point for arc.
   */
  void GetSegmentArc(std::vector<PoseWithCurvature>* vector, double t0,
                     double t1) const {
    const auto start = GetPoint(t0);
    const auto end = GetPoint(t1);

    const auto twist = start.first.Log(end.first);

    if (units::math::abs(twist.dy) > kMaxDy ||
        units::math::abs(twist.dx) > kMaxDx ||
        units::math::abs(twist.dtheta) > kMaxDtheta) {
      GetSegmentArc(vector, t0, (t0 + t1) / 2);
      GetSegmentArc(vector, (t0 + t1) / 2, t1);
    } else {
      vector->push_back(GetPoint(t1));
    }
  }

  friend class QuinticHermiteSplineTest;
  friend class CubicHermiteSplineTest;
};
}  // namespace frc
