/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>
#include <utility>
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
   * Returns 2 cubic control vectors from a set of exterior waypoints and
   * interior translations.
   *
   * @param start             The starting pose.
   * @param interiorWaypoints The interior waypoints.
   * @param end               The ending pose.
   * @return 2 cubic control vectors.
   */
  static std::array<Spline<3>::ControlVector, 2>
  CubicControlVectorsFromWaypoints(
      const Pose2d& start, const std::vector<Translation2d>& interiorWaypoints,
      const Pose2d& end);

  /**
   * Returns quintic control vectors from a set of waypoints.
   *
   * @param waypoints The waypoints
   * @return List of control vectors
   */
  static std::vector<Spline<5>::ControlVector>
  QuinticControlVectorsFromWaypoints(const std::vector<Pose2d>& waypoints);

  /**
   * Returns a set of cubic splines corresponding to the provided control
   * vectors. The user is free to set the direction of the start and end
   * point. The directions for the middle waypoints are determined
   * automatically to ensure continuous curvature throughout the path.
   *
   * The derivation for the algorithm used can be found here:
   * <https://www.uio.no/studier/emner/matnat/ifi/nedlagte-emner/INF-MAT4350/h08/undervisningsmateriale/chap7alecture.pdf>
   *
   * @param start The starting control vector.
   * @param waypoints The middle waypoints. This can be left blank if you
   * only wish to create a path with two waypoints.
   * @param end The ending control vector.
   *
   * @return A vector of cubic hermite splines that interpolate through the
   * provided waypoints.
   */
  static std::vector<CubicHermiteSpline> CubicSplinesFromControlVectors(
      const Spline<3>::ControlVector& start,
      std::vector<Translation2d> waypoints,
      const Spline<3>::ControlVector& end);

  /**
   * Returns a set of quintic splines corresponding to the provided control
   * vectors. The user is free to set the direction of all waypoints. Continuous
   * curvature is guaranteed throughout the path.
   *
   * @param controlVectors The control vectors.
   * @return A vector of quintic hermite splines that interpolate through the
   * provided waypoints.
   */
  static std::vector<QuinticHermiteSpline> QuinticSplinesFromControlVectors(
      const std::vector<Spline<5>::ControlVector>& controlVectors);

 private:
  static Spline<3>::ControlVector CubicControlVector(double scalar,
                                                     const Pose2d& point) {
    return {
        {point.Translation().X().to<double>(), scalar * point.Rotation().Cos()},
        {point.Translation().Y().to<double>(),
         scalar * point.Rotation().Sin()}};
  }

  static Spline<5>::ControlVector QuinticControlVector(double scalar,
                                                       const Pose2d& point) {
    return {{point.Translation().X().to<double>(),
             scalar * point.Rotation().Cos(), 0.0},
            {point.Translation().Y().to<double>(),
             scalar * point.Rotation().Sin(), 0.0}};
  }

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
