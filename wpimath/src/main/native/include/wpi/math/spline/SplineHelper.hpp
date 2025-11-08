// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <vector>

#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/spline/CubicHermiteSpline.h"
#include "frc/spline/QuinticHermiteSpline.h"

namespace frc {
/**
 * Helper class that is used to generate cubic and quintic splines from user
 * provided waypoints.
 */
class WPILIB_DLLEXPORT SplineHelper {
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
  static wpi::array<Spline<3>::ControlVector, 2>
  CubicControlVectorsFromWaypoints(
      const Pose2d& start, const std::vector<Translation2d>& interiorWaypoints,
      const Pose2d& end) {
    double scalar;
    if (interiorWaypoints.empty()) {
      scalar = 1.2 * start.Translation().Distance(end.Translation()).value();
    } else {
      scalar =
          1.2 * start.Translation().Distance(interiorWaypoints.front()).value();
    }
    const auto initialCV = CubicControlVector(scalar, start);
    if (!interiorWaypoints.empty()) {
      scalar =
          1.2 * end.Translation().Distance(interiorWaypoints.back()).value();
    }
    const auto finalCV = CubicControlVector(scalar, end);
    return {initialCV, finalCV};
  }

  /**
   * Returns quintic splines from a set of waypoints.
   *
   * @param waypoints The waypoints
   * @return List of quintic splines.
   */
  static std::vector<QuinticHermiteSpline> QuinticSplinesFromWaypoints(
      const std::vector<Pose2d>& waypoints) {
    std::vector<QuinticHermiteSpline> splines;
    splines.reserve(waypoints.size() - 1);
    for (size_t i = 0; i < waypoints.size() - 1; ++i) {
      auto& p0 = waypoints[i];
      auto& p1 = waypoints[i + 1];

      // This just makes the splines look better.
      const auto scalar =
          1.2 * p0.Translation().Distance(p1.Translation()).value();

      auto controlVectorA = QuinticControlVector(scalar, p0);
      auto controlVectorB = QuinticControlVector(scalar, p1);
      splines.emplace_back(controlVectorA.x, controlVectorB.x, controlVectorA.y,
                           controlVectorB.y);
    }
    return splines;
  }

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
      const Spline<3>::ControlVector& end) {
    std::vector<CubicHermiteSpline> splines;

    wpi::array<double, 2> xInitial = start.x;
    wpi::array<double, 2> yInitial = start.y;
    wpi::array<double, 2> xFinal = end.x;
    wpi::array<double, 2> yFinal = end.y;

    if (waypoints.size() > 1) {
      waypoints.emplace(waypoints.begin(),
                        Translation2d{units::meter_t{xInitial[0]},
                                      units::meter_t{yInitial[0]}});
      waypoints.emplace_back(
          Translation2d{units::meter_t{xFinal[0]}, units::meter_t{yFinal[0]}});

      // Populate tridiagonal system for clamped cubic
      /* See:
      https://www.uio.no/studier/emner/matnat/ifi/nedlagte-emner/INF-MAT4350/h08
      /undervisningsmateriale/chap7alecture.pdf
      */

      // Above-diagonal of tridiagonal matrix, zero-padded
      std::vector<double> a;
      // Diagonal of tridiagonal matrix
      std::vector<double> b(waypoints.size() - 2, 4.0);
      // Below-diagonal of tridiagonal matrix, zero-padded
      std::vector<double> c;
      // rhs vectors
      std::vector<double> dx, dy;
      // solution vectors
      std::vector<double> fx(waypoints.size() - 2, 0.0),
          fy(waypoints.size() - 2, 0.0);

      // populate above-diagonal and below-diagonal vectors
      a.emplace_back(0);
      for (size_t i = 0; i < waypoints.size() - 3; ++i) {
        a.emplace_back(1);
        c.emplace_back(1);
      }
      c.emplace_back(0);

      // populate rhs vectors
      dx.emplace_back(
          3 * (waypoints[2].X().value() - waypoints[0].X().value()) -
          xInitial[1]);
      dy.emplace_back(
          3 * (waypoints[2].Y().value() - waypoints[0].Y().value()) -
          yInitial[1]);
      if (waypoints.size() > 4) {
        for (size_t i = 1; i <= waypoints.size() - 4; ++i) {
          // dx and dy represent the derivatives of the internal waypoints. The
          // derivative of the second internal waypoint should involve the third
          // and first internal waypoint, which have indices of 1 and 3 in the
          // waypoints list (which contains ALL waypoints).
          dx.emplace_back(
              3 * (waypoints[i + 2].X().value() - waypoints[i].X().value()));
          dy.emplace_back(
              3 * (waypoints[i + 2].Y().value() - waypoints[i].Y().value()));
        }
      }
      dx.emplace_back(3 * (waypoints[waypoints.size() - 1].X().value() -
                           waypoints[waypoints.size() - 3].X().value()) -
                      xFinal[1]);
      dy.emplace_back(3 * (waypoints[waypoints.size() - 1].Y().value() -
                           waypoints[waypoints.size() - 3].Y().value()) -
                      yFinal[1]);

      // Compute solution to tridiagonal system
      ThomasAlgorithm(a, b, c, dx, &fx);
      ThomasAlgorithm(a, b, c, dy, &fy);

      fx.emplace(fx.begin(), xInitial[1]);
      fx.emplace_back(xFinal[1]);
      fy.emplace(fy.begin(), yInitial[1]);
      fy.emplace_back(yFinal[1]);

      for (size_t i = 0; i < fx.size() - 1; ++i) {
        // Create the spline.
        const CubicHermiteSpline spline{
            {waypoints[i].X().value(), fx[i]},
            {waypoints[i + 1].X().value(), fx[i + 1]},
            {waypoints[i].Y().value(), fy[i]},
            {waypoints[i + 1].Y().value(), fy[i + 1]}};

        splines.push_back(spline);
      }
    } else if (waypoints.size() == 1) {
      const double xDeriv =
          (3 * (xFinal[0] - xInitial[0]) - xFinal[1] - xInitial[1]) / 4.0;
      const double yDeriv =
          (3 * (yFinal[0] - yInitial[0]) - yFinal[1] - yInitial[1]) / 4.0;

      wpi::array<double, 2> midXControlVector{waypoints[0].X().value(), xDeriv};
      wpi::array<double, 2> midYControlVector{waypoints[0].Y().value(), yDeriv};

      splines.emplace_back(xInitial, midXControlVector, yInitial,
                           midYControlVector);
      splines.emplace_back(midXControlVector, xFinal, midYControlVector,
                           yFinal);

    } else {
      // Create the spline.
      const CubicHermiteSpline spline{xInitial, xFinal, yInitial, yFinal};
      splines.push_back(spline);
    }

    return splines;
  }

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
      const std::vector<Spline<5>::ControlVector>& controlVectors) {
    std::vector<QuinticHermiteSpline> splines;
    for (size_t i = 0; i < controlVectors.size() - 1; ++i) {
      auto& xInitial = controlVectors[i].x;
      auto& yInitial = controlVectors[i].y;
      auto& xFinal = controlVectors[i + 1].x;
      auto& yFinal = controlVectors[i + 1].y;
      splines.emplace_back(xInitial, xFinal, yInitial, yFinal);
    }
    return splines;
  }

  /**
   * Optimizes the curvature of 2 or more quintic splines at knot points.
   * Overall, this reduces the integral of the absolute value of the second
   * derivative across the set of splines.
   *
   * @param splines A vector of un-optimized quintic splines.
   * @return A vector of optimized quintic splines.
   */
  static std::vector<QuinticHermiteSpline> OptimizeCurvature(
      const std::vector<QuinticHermiteSpline>& splines) {
    // If there's only one spline in the vector, we can't optimize anything so
    // just return that.
    if (splines.size() < 2) {
      return splines;
    }

    // Implements Section 4.1.2 of
    // http://www2.informatik.uni-freiburg.de/~lau/students/Sprunk2008.pdf.

    // Cubic splines minimize the integral of the second derivative's absolute
    // value. Therefore, we can create cubic splines with the same 0th and 1st
    // derivatives and the provided quintic splines, find the second derivative
    // of those cubic splines and then use a weighted average for the second
    // derivatives of the quintic splines.

    std::vector<QuinticHermiteSpline> optimizedSplines;
    optimizedSplines.reserve(splines.size());
    optimizedSplines.push_back(splines[0]);

    for (size_t i = 0; i < splines.size() - 1; ++i) {
      const auto& a = splines[i];
      const auto& b = splines[i + 1];

      // Get the control vectors that created the quintic splines above.
      const auto& aInitial = a.GetInitialControlVector();
      const auto& aFinal = a.GetFinalControlVector();
      const auto& bInitial = b.GetInitialControlVector();
      const auto& bFinal = b.GetFinalControlVector();

      // Create cubic splines with the same control vectors.
      auto Trim = [](const wpi::array<double, 3>& a) {
        return wpi::array<double, 2>{a[0], a[1]};
      };
      CubicHermiteSpline ca{Trim(aInitial.x), Trim(aFinal.x), Trim(aInitial.y),
                            Trim(aFinal.y)};
      CubicHermiteSpline cb{Trim(bInitial.x), Trim(bFinal.x), Trim(bInitial.y),
                            Trim(bFinal.y)};

      // Calculate the second derivatives at the knot points.
      frc::Vectord<4> bases{1.0, 1.0, 1.0, 1.0};
      frc::Vectord<6> combinedA = ca.Coefficients() * bases;

      double ddxA = combinedA(4);
      double ddyA = combinedA(5);
      double ddxB = cb.Coefficients()(4, 1);
      double ddyB = cb.Coefficients()(5, 1);

      // Calculate the parameters for weighted average.
      double dAB =
          std::hypot(aFinal.x[0] - aInitial.x[0], aFinal.y[0] - aInitial.y[0]);
      double dBC =
          std::hypot(bFinal.x[0] - bInitial.x[0], bFinal.y[0] - bInitial.y[0]);
      double alpha = dBC / (dAB + dBC);
      double beta = dAB / (dAB + dBC);

      // Calculate the weighted average.
      double ddx = alpha * ddxA + beta * ddxB;
      double ddy = alpha * ddyA + beta * ddyB;

      // Create new splines.
      optimizedSplines[i] = {aInitial.x,
                             {aFinal.x[0], aFinal.x[1], ddx},
                             aInitial.y,
                             {aFinal.y[0], aFinal.y[1], ddy}};
      optimizedSplines.push_back({{bInitial.x[0], bInitial.x[1], ddx},
                                  bFinal.x,
                                  {bInitial.y[0], bInitial.y[1], ddy},
                                  bFinal.y});
    }

    return optimizedSplines;
  }

 private:
  static Spline<3>::ControlVector CubicControlVector(double scalar,
                                                     const Pose2d& point) {
    return {{point.X().value(), scalar * point.Rotation().Cos()},
            {point.Y().value(), scalar * point.Rotation().Sin()}};
  }

  static Spline<5>::ControlVector QuinticControlVector(double scalar,
                                                       const Pose2d& point) {
    return {{point.X().value(), scalar * point.Rotation().Cos(), 0.0},
            {point.Y().value(), scalar * point.Rotation().Sin(), 0.0}};
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
                              std::vector<double>* solutionVector) {
    auto& f = *solutionVector;
    size_t N = d.size();

    // Create the temporary vectors
    // Note that this is inefficient as it is possible to call
    // this function many times. A better implementation would
    // pass these temporary matrices by non-const reference to
    // save excess allocation and deallocation
    std::vector<double> c_star(N, 0.0);
    std::vector<double> d_star(N, 0.0);

    // This updates the coefficients in the first row
    // Note that we should be checking for division by zero here
    c_star[0] = c[0] / b[0];
    d_star[0] = d[0] / b[0];

    // Create the c_star and d_star coefficients in the forward sweep
    for (size_t i = 1; i < N; ++i) {
      double m = 1.0 / (b[i] - a[i] * c_star[i - 1]);
      c_star[i] = c[i] * m;
      d_star[i] = (d[i] - a[i] * d_star[i - 1]) * m;
    }

    f[N - 1] = d_star[N - 1];
    // This is the reverse sweep, used to update the solution vector f
    for (int i = N - 2; i >= 0; i--) {
      f[i] = d_star[i] - c_star[i] * f[i + 1];
    }
  }
};
}  // namespace frc
