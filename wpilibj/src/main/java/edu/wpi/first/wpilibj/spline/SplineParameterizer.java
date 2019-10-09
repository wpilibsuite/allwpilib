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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package edu.wpi.first.wpilibj.spline;

import java.util.ArrayList;
import java.util.List;

/**
 * Class used to parameterize a spline by its arc length.
 */
public final class SplineParameterizer {
  private static final double kMaxDx = 0.127;
  private static final double kMaxDy = 0.00127;
  private static final double kMaxDtheta = 0.0872;

  /**
   * Private constructor because this is a utility class.
   */
  private SplineParameterizer() {
  }

  /**
   * Parameterizes the spline. This method breaks up the spline into various
   * arcs until their dx, dy, and dtheta are within specific tolerances.
   *
   * @param spline The spline to parameterize.
   * @return A vector of poses and curvatures that represents various points on the spline.
   */
  public static List<PoseWithCurvature> parameterize(Spline spline) {
    return parameterize(spline, 0.0, 1.0);
  }

  /**
   * Parameterizes the spline. This method breaks up the spline into various
   * arcs until their dx, dy, and dtheta are within specific tolerances.
   *
   * @param spline The spline to parameterize.
   * @param t0     Starting internal spline parameter. It is recommended to use 0.0.
   * @param t1     Ending internal spline parameter. It is recommended to use 1.0.
   * @return A vector of poses and curvatures that represents various points on the spline.
   */
  public static List<PoseWithCurvature> parameterize(Spline spline, double t0, double t1) {
    var arr = new ArrayList<PoseWithCurvature>();

    // The parameterization does not add the first initial point. Let's add
    // that.
    arr.add(spline.getPoint(t0));

    getSegmentArc(spline, arr, t0, t1);
    return arr;
  }

  /**
   * Breaks up the spline into arcs until the dx, dy, and theta of each arc is
   * within tolerance.
   *
   * @param spline The spline to parameterize.
   * @param vector Pointer to vector of poses.
   * @param t0     Starting point for arc.
   * @param t1     Ending point for arc.
   */
  private static void getSegmentArc(Spline spline, List<PoseWithCurvature> vector,
                                    double t0, double t1) {
    final var start = spline.getPoint(t0);
    final var end = spline.getPoint(t1);

    final var twist = start.poseMeters.log(end.poseMeters);

    if (Math.abs(twist.dy) > kMaxDy || Math.abs(twist.dx) > kMaxDx
        || Math.abs(twist.dtheta) > kMaxDtheta) {
      getSegmentArc(spline, vector, t0, (t0 + t1) / 2);
      getSegmentArc(spline, vector, (t0 + t1) / 2, t1);
    } else {
      vector.add(spline.getPoint(t1));
    }
  }
}
