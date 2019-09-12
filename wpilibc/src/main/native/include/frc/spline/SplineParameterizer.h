/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*
 * MIT License
 *
 * Copyright (c) 2018 Team 254
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <frc/spline/Spline.h>

#include <utility>
#include <vector>

#include <units/units.h>

namespace frc {

/**
 * Class used to parameterize a spline by its arc length.
 */
class SplineParameterizer {
 public:
  using PoseWithCurvature = std::pair<Pose2d, curvature_t>;

  /**
   * Parameterizes the spline. This method breaks up the spline into various
   * arcs until their dx, dy, and dtheta are within specific tolerances.
   *
   * @param spline The spline to parameterize.
   * @param t0 Starting internal spline parameter. It is recommended to leave
   * this as default.
   * @param t1 Ending internal spline parameter. It is recommended to leave this
   * as default.
   *
   * @return A vector of poses and curvatures that represents various points on
   * the spline.
   */
  template <int Dim>
  static std::vector<PoseWithCurvature> Parameterize(const Spline<Dim>& spline,
                                                     double t0 = 0.0,
                                                     double t1 = 1.0) {
    std::vector<PoseWithCurvature> arr;

    // The parameterization does not add the first initial point. Let's add
    // that.
    arr.push_back(spline.GetPoint(t0));

    GetSegmentArc(spline, &arr, t0, t1);
    return arr;
  }

 private:
  // Constraints for spline parameterization.
  static constexpr units::meter_t kMaxDx = 5_in;
  static constexpr units::meter_t kMaxDy = 0.05_in;
  static constexpr units::radian_t kMaxDtheta = 0.0872_rad;

  /**
   * Breaks up the spline into arcs until the dx, dy, and theta of each arc is
   * within tolerance.
   *
   * @param spline The spline to parameterize.
   * @param vector Pointer to vector of poses.
   * @param t0 Starting point for arc.
   * @param t1 Ending point for arc.
   */
  template <int Dim>
  static void GetSegmentArc(const Spline<Dim>& spline,
                            std::vector<PoseWithCurvature>* vector, double t0,
                            double t1) {
    const auto start = spline.GetPoint(t0);
    const auto end = spline.GetPoint(t1);

    const auto twist = start.first.Log(end.first);

    if (units::math::abs(twist.dy) > kMaxDy ||
        units::math::abs(twist.dx) > kMaxDx ||
        units::math::abs(twist.dtheta) > kMaxDtheta) {
      GetSegmentArc(spline, vector, t0, (t0 + t1) / 2);
      GetSegmentArc(spline, vector, (t0 + t1) / 2, t1);
    } else {
      vector->push_back(spline.GetPoint(t1));
    }
  }

  friend class CubicHermiteSplineTest;
  friend class QuinticHermiteSplineTest;
};
}  // namespace frc
