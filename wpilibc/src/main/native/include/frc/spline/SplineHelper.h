/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include "frc/spline/CubicHermiteSpline.h"
#include "frc/spline/QuinticHermiteSpline.h"

namespace frc {
/**
 * Helper class that is used to generate cubic and quintic splines from user
 * provided waypoints.
 */
class SplineHelper {
 public:
  /**
   * Returns a set of cubic splines corresponding to the provided waypoints. The
   * user is free to set the direction of the start and end point. The
   * directions for the middle waypoints are determined automatically to ensure
   * continuous curvature throughout the path.
   *
   * The derivation for the algorithm used can be found here:
   * <https://www.uio.no/studier/emner/matnat/ifi/nedlagte-emner/INF-MAT4350/h08/undervisningsmateriale/chap7alecture.pdf>
   *
   * @param start The starting waypoint.
   * @param waypoints The middle waypoints. This can be left blank if you only
   * wish to create a path with two waypoints.
   * @param end The ending waypoint.
   *
   * @return A vector of cubic hermite splines that interpolate through the
   * provided waypoints.
   */
  static std::vector<CubicHermiteSpline> CubicSplinesFromWaypoints(
      const Pose2d& start, std::vector<Translation2d> waypoints,
      const Pose2d& end);

  /**
   * Returns a set of quintic splines corresponding to the provided waypoints.
   * The user is free to set the direction of all waypoints. Continuous
   * curvature is guaranteed throughout the path.
   *
   * @param waypoints The waypoints.
   * @return A vector of quintic hermite splines that interpolate through the
   * provided waypoints.
   */
  static std::vector<QuinticHermiteSpline> QuinticSplinesFromWaypoints(
      const std::vector<Pose2d>& waypoints);

 private:
  /**
   * Thomas algorithm for solving tridiagonal systems Af = d.
   *
   * @param a the values of A above the diagonal
   * @param b the values of A on the diagonal
   * @param c the values of A below the diagonal
   * @param d the vector on the rhs
   * @param solutionVector the unknown (solution) vector, modified in-place
   */
  static void ThomasAlgorithm(const std::vector<double>& a,
                              const std::vector<double>& b,
                              const std::vector<double>& c,
                              const std::vector<double>& d,
                              std::vector<double>* solutionVector);
};
}  // namespace frc
