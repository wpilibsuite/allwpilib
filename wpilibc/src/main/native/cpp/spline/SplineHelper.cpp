/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/spline/SplineHelper.h"

#include <cstddef>

using namespace frc;

std::vector<CubicHermiteSpline> SplineHelper::CubicSplinesFromControlVectors(
    const Spline<3>::ControlVector& start, std::vector<Translation2d> waypoints,
    const Spline<3>::ControlVector& end) {
  std::vector<CubicHermiteSpline> splines;

  std::array<double, 2> xInitial = start.x;
  std::array<double, 2> yInitial = start.y;
  std::array<double, 2> xFinal = end.x;
  std::array<double, 2> yFinal = end.y;

  if (waypoints.size() > 1) {
    waypoints.emplace(waypoints.begin(),
                      Translation2d{units::meter_t(xInitial[0]),
                                    units::meter_t(yInitial[0])});
    waypoints.emplace_back(
        Translation2d{units::meter_t(xFinal[0]), units::meter_t(yFinal[0])});

    std::vector<double> a;
    std::vector<double> b(waypoints.size() - 2, 4.0);
    std::vector<double> c;
    std::vector<double> dx, dy;
    std::vector<double> fx(waypoints.size() - 2, 0.0),
        fy(waypoints.size() - 2, 0.0);

    a.emplace_back(0);
    for (size_t i = 0; i < waypoints.size() - 3; ++i) {
      a.emplace_back(1);
      c.emplace_back(1);
    }
    c.emplace_back(0);

    dx.emplace_back(
        3 * (waypoints[2].X().to<double>() - waypoints[0].X().to<double>()) -
        xInitial[1]);
    dy.emplace_back(
        3 * (waypoints[2].Y().to<double>() - waypoints[0].Y().to<double>()) -
        yInitial[1]);
    if (waypoints.size() > 4) {
      for (size_t i = 1; i <= waypoints.size() - 4; ++i) {
        dx.emplace_back(3 * (waypoints[i + 1].X().to<double>() -
                             waypoints[i - 1].X().to<double>()));
        dy.emplace_back(3 * (waypoints[i + 1].Y().to<double>() -
                             waypoints[i - 1].Y().to<double>()));
      }
    }
    dx.emplace_back(3 * (waypoints[waypoints.size() - 1].X().to<double>() -
                         waypoints[waypoints.size() - 3].X().to<double>()) -
                    xFinal[1]);
    dy.emplace_back(3 * (waypoints[waypoints.size() - 1].Y().to<double>() -
                         waypoints[waypoints.size() - 3].Y().to<double>()) -
                    yFinal[1]);

    ThomasAlgorithm(a, b, c, dx, &fx);
    ThomasAlgorithm(a, b, c, dy, &fy);

    fx.emplace(fx.begin(), xInitial[1]);
    fx.emplace_back(xFinal[1]);
    fy.emplace(fy.begin(), yInitial[1]);
    fy.emplace_back(yFinal[1]);

    for (size_t i = 0; i < fx.size() - 1; ++i) {
      // Create the spline.
      const CubicHermiteSpline spline{
          {waypoints[i].X().to<double>(), fx[i]},
          {waypoints[i + 1].X().to<double>(), fx[i + 1]},
          {waypoints[i].Y().to<double>(), fy[i]},
          {waypoints[i + 1].Y().to<double>(), fy[i + 1]}};

      splines.push_back(spline);
    }
  } else if (waypoints.size() == 1) {
    const double xDeriv =
        (3 * (xFinal[0] - xInitial[0]) - xFinal[1] - xInitial[1]) / 4.0;
    const double yDeriv =
        (3 * (yFinal[0] - yInitial[0]) - yFinal[1] - yInitial[1]) / 4.0;

    std::array<double, 2> midXControlVector{waypoints[0].X().to<double>(),
                                            xDeriv};
    std::array<double, 2> midYControlVector{waypoints[0].Y().to<double>(),
                                            yDeriv};

    splines.emplace_back(xInitial, midXControlVector, yInitial,
                         midYControlVector);
    splines.emplace_back(midXControlVector, xFinal, midYControlVector, yFinal);

  } else {
    // Create the spline.
    const CubicHermiteSpline spline{xInitial, xFinal, yInitial, yFinal};
    splines.push_back(spline);
  }

  return splines;
}

std::vector<QuinticHermiteSpline>
SplineHelper::QuinticSplinesFromControlVectors(
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

std::array<Spline<3>::ControlVector, 2>
SplineHelper::CubicControlVectorsFromWaypoints(
    const Pose2d& start, const std::vector<Translation2d>& interiorWaypoints,
    const Pose2d& end) {
  double scalar;
  if (interiorWaypoints.empty()) {
    scalar = 1.2 * start.Translation().Distance(end.Translation()).to<double>();
  } else {
    scalar =
        1.2 *
        start.Translation().Distance(interiorWaypoints.front()).to<double>();
  }
  const auto initialCV = CubicControlVector(scalar, start);
  if (!interiorWaypoints.empty()) {
    scalar =
        1.2 * end.Translation().Distance(interiorWaypoints.back()).to<double>();
  }
  const auto finalCV = CubicControlVector(scalar, end);
  return {initialCV, finalCV};
}

std::vector<Spline<5>::ControlVector>
SplineHelper::QuinticControlVectorsFromWaypoints(
    const std::vector<Pose2d>& waypoints) {
  std::vector<Spline<5>::ControlVector> vectors;
  for (size_t i = 0; i < waypoints.size() - 1; ++i) {
    auto& p0 = waypoints[i];
    auto& p1 = waypoints[i + 1];

    // This just makes the splines look better.
    const auto scalar =
        1.2 * p0.Translation().Distance(p1.Translation()).to<double>();

    vectors.push_back(QuinticControlVector(scalar, p0));
    vectors.push_back(QuinticControlVector(scalar, p1));
  }
  return vectors;
}

void SplineHelper::ThomasAlgorithm(const std::vector<double>& a,
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
