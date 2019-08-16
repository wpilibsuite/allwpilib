/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.spline;

import java.util.ArrayList;
import java.util.List;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;

/**
 * Represents a two-dimensional parametric spline that interpolates between two
 * points.
 */
public abstract class Spline {
  private static final double kMaxDx = 0.127;
  private static final double kMaxDy = 0.00127;
  private static final double kMaxDtheta = 0.0872;

  private final int m_degree;

  /**
   * Constructs a spline with the given degree.
   *
   * @param degree The degree of the spline.
   */
  Spline(int degree) {
    m_degree = degree;
  }

  /**
   * Parameterizes the spline. This method breaks up the spline into various
   * arcs until their dx, dy, and dtheta are within specific tolerances.
   *
   * @return A vector of poses and curvatures that represents various points on the spline.
   */
  public List<PoseWithCurvature> parameterize() {
    return parameterize(0.0, 1.0);
  }

  /**
   * Parameterizes the spline. This method breaks up the spline into various
   * arcs until their dx, dy, and dtheta are within specific tolerances.
   *
   * @param t0 Starting internal spline parameter. It is recommended to use 0.0.
   * @param t1 Ending internal spline parameter. It is recommended to use 1.0.
   * @return A vector of poses and curvatures that represents various points on the spline.
   */
  public List<PoseWithCurvature> parameterize(double t0, double t1) {
    var arr = new ArrayList<PoseWithCurvature>();
    getSegmentArc(arr, t0, t1);
    return arr;
  }

  /**
   * Returns the coefficients of the spline.
   *
   * @return The coefficients of the spline.
   */
  protected abstract SimpleMatrix getCoefficients();

  @SuppressWarnings({"ParameterName", "MultipleVariableDeclarations"})
  public PoseWithCurvature getPoint(double t) {
    SimpleMatrix polynomialBases = new SimpleMatrix(m_degree + 1, 1);
    final var coefficients = getCoefficients();

    // Populate the polynomial bases.
    for (int i = 0; i <= m_degree; i++) {
      polynomialBases.set(i, 0, Math.pow(t, m_degree - i));
    }

    // This simply multiplies by the coefficients. We need to divide out t some
    // n number of times where n is the derivative we want to take.
    SimpleMatrix combined = coefficients.mult(polynomialBases);

    // Get x and y
    final double x = combined.get(0, 0);
    final double y = combined.get(1, 0);

    double dx, dy, ddx, ddy;

    if (t == 0) {
      dx = coefficients.get(2, m_degree - 1);
      dy = coefficients.get(3, m_degree - 1);
      ddx = coefficients.get(4, m_degree - 2);
      ddy = coefficients.get(5, m_degree - 2);
    } else {
      // Divide out t once for first derivative.
      dx = combined.get(2, 0) / t;
      dy = combined.get(3, 0) / t;

      // Divide out t twice for second derivative.
      ddx = combined.get(4, 0) / t / t;
      ddy = combined.get(5, 0) / t / t;
    }

    // Find the curvature.
    final double curvature =
        (dx * ddy - ddx * dy) / ((dx * dx + dy * dy) * Math.hypot(dx, dy));

    return new PoseWithCurvature(
        new Pose2d(x, y, new Rotation2d(dx, dy)),
        curvature
    );
  }

  /**
   * Breaks up the spline into arcs until the dx, dy, and theta of each arc is
   * within tolerance.
   *
   * @param vector Pointer to vector of poses.
   * @param t0     Starting point for arc.
   * @param t1     Ending point for arc.
   */
  private void getSegmentArc(ArrayList<PoseWithCurvature> vector, double t0, double t1) {
    final var start = getPoint(t0);
    final var end = getPoint(t1);

    final var twist = start.pose.log(end.pose);

    if (Math.abs(twist.dy) > kMaxDy || Math.abs(twist.dx) > kMaxDx
        || Math.abs(twist.dtheta) > kMaxDtheta) {
      getSegmentArc(vector, t0, (t0 + t1) / 2);
      getSegmentArc(vector, (t0 + t1) / 2, t1);
    } else {
      vector.add(getPoint(t1));
    }
  }
}
