/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/spline/SplineHelper.h"

#include <cstddef>

using namespace frc;

std::vector<CubicHermiteSpline> SplineHelper::CubicSplinesFromWaypoints(
    const Pose2d& start, std::vector<Translation2d> waypoints,
    const Pose2d& end) {
  std::vector<CubicHermiteSpline> splines;

  double scalar;
  // This just makes the splines look better.
  if (waypoints.empty()) {
    scalar = 1.2 * start.Translation().Distance(end.Translation()).to<double>();
  } else {
    scalar = 1.2 * start.Translation().Distance(waypoints.front()).to<double>();
  }

  std::array<double, 2> startXControlVector{
      start.Translation().X().to<double>(), start.Rotation().Cos() * scalar};

  std::array<double, 2> startYControlVector{
      start.Translation().Y().to<double>(), start.Rotation().Sin() * scalar};

  // This just makes the splines look better.
  if (!waypoints.empty()) {
    scalar = 1.2 * end.Translation().Distance(waypoints.back()).to<double>();
  }

  std::array<double, 2> endXControlVector{end.Translation().X().to<double>(),
                                          end.Rotation().Cos() * scalar};

  std::array<double, 2> endYControlVector{end.Translation().Y().to<double>(),
                                          end.Rotation().Sin() * scalar};

  if (waypoints.size() > 1) {
    waypoints.emplace(waypoints.begin(), start.Translation());
    waypoints.emplace_back(end.Translation());

    std::vector<double> a;
    std::vector<double> b(waypoints.size() - 2, 4.0);
    std::vector<double> c;
    std::vector<double> dx, dy;
    std::vector<double> fx(waypoints.size() - 2, 0.0),
        fy(waypoints.size() - 2, 0.0);

    a.emplace_back(0);
    for (unsigned int i = 0; i < waypoints.size() - 3; i++) {
      a.emplace_back(1);
      c.emplace_back(1);
    }
    c.emplace_back(0);

    dx.emplace_back(
        3 * (waypoints[2].X().to<double>() - waypoints[0].X().to<double>()) -
        startXControlVector[1]);
    dy.emplace_back(
        3 * (waypoints[2].Y().to<double>() - waypoints[0].Y().to<double>()) -
        startYControlVector[1]);
    if (waypoints.size() > 4) {
      for (unsigned int i = 1; i <= waypoints.size() - 4; i++) {
        dx.emplace_back(3 * (waypoints[i + 1].X().to<double>() -
                             waypoints[i - 1].X().to<double>()));
        dy.emplace_back(3 * (waypoints[i + 1].Y().to<double>() -
                             waypoints[i - 1].Y().to<double>()));
      }
    }
    dx.emplace_back(3 * (waypoints[waypoints.size() - 1].X().to<double>() -
                         waypoints[waypoints.size() - 3].X().to<double>()) -
                    endXControlVector[1]);
    dy.emplace_back(3 * (waypoints[waypoints.size() - 1].Y().to<double>() -
                         waypoints[waypoints.size() - 3].Y().to<double>()) -
                    endYControlVector[1]);

    ThomasAlgorithm(a, b, c, dx, &fx);
    ThomasAlgorithm(a, b, c, dy, &fy);

    fx.emplace(fx.begin(), startXControlVector[1]);
    fx.emplace_back(endXControlVector[1]);
    fy.emplace(fy.begin(), startYControlVector[1]);
    fy.emplace_back(endYControlVector[1]);

    for (unsigned int i = 0; i < fx.size() - 1; i++) {
      // Create the spline.
      const CubicHermiteSpline spline{
          {waypoints[i].X().to<double>(), fx[i]},
          {waypoints[i + 1].X().to<double>(), fx[i + 1]},
          {waypoints[i].Y().to<double>(), fy[i]},
          {waypoints[i + 1].Y().to<double>(), fy[i + 1]}};

      splines.push_back(spline);
    }
  } else if (waypoints.size() == 1) {
    const double xDeriv = (3 * (end.Translation().X().to<double>() -
                                start.Translation().X().to<double>()) -
                           endXControlVector[1] - startXControlVector[1]) /
                          4.0;
    const double yDeriv = (3 * (end.Translation().Y().to<double>() -
                                start.Translation().Y().to<double>()) -
                           endYControlVector[1] - startYControlVector[1]) /
                          4.0;

    std::array<double, 2> midXControlVector{waypoints[0].X().to<double>(),
                                            xDeriv};
    std::array<double, 2> midYControlVector{waypoints[0].Y().to<double>(),
                                            yDeriv};

    splines.emplace_back(startXControlVector, midXControlVector,
                         startYControlVector, midYControlVector);
    splines.emplace_back(midXControlVector, endXControlVector,
                         midYControlVector, endYControlVector);

  } else {
    // Create the spline.
    const CubicHermiteSpline spline{startXControlVector, endXControlVector,
                                    startYControlVector, endYControlVector};
    splines.push_back(spline);
  }

  return splines;
}

std::vector<QuinticHermiteSpline> SplineHelper::QuinticSplinesFromWaypoints(
    const std::vector<Pose2d>& waypoints) {
  std::vector<QuinticHermiteSpline> splines;
  for (unsigned int i = 0; i < waypoints.size() - 1; i++) {
    auto& p0 = waypoints[i];
    auto& p1 = waypoints[i + 1];

    // This just makes the splines look better.
    const auto scalar =
        1.2 * p0.Translation().Distance(p1.Translation()).to<double>();

    const std::array<double, 3> xInitialControlVector{
        p0.Translation().X().to<double>(), p0.Rotation().Cos() * scalar, 0.0};
    const std::array<double, 3> xFinalControlVector{
        p1.Translation().X().to<double>(), p1.Rotation().Cos() * scalar, 0.0};
    const std::array<double, 3> yInitialControlVector{
        p0.Translation().Y().to<double>(), p0.Rotation().Sin() * scalar, 0.0};
    const std::array<double, 3> yFinalControlVector{
        p1.Translation().Y().to<double>(), p1.Rotation().Sin() * scalar, 0.0};

    splines.emplace_back(xInitialControlVector, xFinalControlVector,
                         yInitialControlVector, yFinalControlVector);
  }
  return splines;
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
  for (unsigned int i = 1; i < N; i++) {
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
