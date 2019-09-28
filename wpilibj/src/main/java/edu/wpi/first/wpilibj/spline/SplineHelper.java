/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.spline;

import java.util.Arrays;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;

public final class SplineHelper {
  /**
   * Private constructor because this is a utility class.
   */
  private SplineHelper() {
  }

  /**
   * Returns a set of cubic splines corresponding to the provided waypoints. The
   * user is free to set the direction of the start and end point. The
   * directions for the middle waypoints are determined automatically to ensure
   * continuous curvature throughout the path.
   *
   * @param start     The starting waypoint.
   * @param waypoints The middle waypoints. This can be left blank if you only
   *                  wish to create a path with two waypoints.
   * @param end       The ending waypoint.
   * @return A vector of cubic hermite splines that interpolate through the
   *         provided waypoints.
   */
  @SuppressWarnings({"LocalVariableName", "PMD.ExcessiveMethodLength",
      "PMD.AvoidInstantiatingObjectsInLoops"})
  public static CubicHermiteSpline[] getCubicSplinesFromWaypoints(
      Pose2d start, Translation2d[] waypoints, Pose2d end) {

    CubicHermiteSpline[] splines = new CubicHermiteSpline[waypoints.length + 1];

    double scalar;
    // This just makes the splines look better.
    if (waypoints.length == 0) {
      scalar = 1.2 * start.getTranslation().getDistance(end.getTranslation());
    } else {
      scalar = 1.2 * start.getTranslation().getDistance(waypoints[0]);
    }

    double[] xInitialControlVector
        = {start.getTranslation().getX(), scalar * start.getRotation().getCos()};
    double[] yInitialControlVector
        = {start.getTranslation().getY(), scalar * start.getRotation().getSin()};

    // This just makes the splines look better.
    if (waypoints.length != 0) {
      scalar = 1.2 * end.getTranslation().getDistance(waypoints[waypoints.length - 1]);
    }

    double[] xFinalControlVector
        = {end.getTranslation().getX(), scalar * end.getRotation().getCos()};
    double[] yFinalControlVector
        = {end.getTranslation().getY(), scalar * end.getRotation().getSin()};

    if (waypoints.length > 1) {
      Translation2d[] newWaypts = new Translation2d[waypoints.length + 2];

      // Create an array of all waypoints, including the start and end.
      newWaypts[0] = start.getTranslation();
      System.arraycopy(waypoints, 0, newWaypts, 1, waypoints.length);
      newWaypts[newWaypts.length - 1] = end.getTranslation();

      final double[] a = new double[1 + newWaypts.length - 3];

      final double[] b = new double[newWaypts.length - 2];
      Arrays.fill(b, 4.0);

      final double[] c = new double[1 + newWaypts.length - 3];

      final double[] dx = new double[2 + newWaypts.length - 4];
      final double[] dy = new double[2 + newWaypts.length - 4];

      final double[] fx = new double[newWaypts.length - 2];
      final double[] fy = new double[newWaypts.length - 2];

      a[0] = 0.0;
      for (int i = 0; i < newWaypts.length - 3; i++) {
        a[i + 1] = 1;
        c[i] = 1;
      }
      c[c.length - 1] = 0.0;

      dx[0] = 3 * (newWaypts[2].getX() - newWaypts[0].getX()) - xInitialControlVector[1];
      dy[0] = 3 * (newWaypts[2].getY() - newWaypts[0].getY()) - yInitialControlVector[1];

      if (newWaypts.length > 4) {
        for (int i = 1; i <= newWaypts.length; i++) {
          dx[i] = newWaypts[i + 1].getX() - newWaypts[i - 1].getX();
          dy[i] = newWaypts[i + 1].getY() - newWaypts[i - 1].getY();
        }
      }

      dx[dx.length - 1] = 3 * (newWaypts[newWaypts.length - 1].getX()
          - newWaypts[newWaypts.length - 3].getX()) - xFinalControlVector[1];
      dy[dy.length - 1] = 3 * (newWaypts[newWaypts.length - 1].getY()
          - newWaypts[newWaypts.length - 3].getY()) - yFinalControlVector[1];

      thomasAlgorithm(a, b, c, dx, fx);
      thomasAlgorithm(a, b, c, dy, fy);

      double[] newFx = new double[fx.length + 2];
      double[] newFy = new double[fy.length + 2];

      newFx[0] = xInitialControlVector[1];
      newFy[0] = yInitialControlVector[1];
      System.arraycopy(fx, 0, newFx, 1, fx.length);
      System.arraycopy(fy, 0, newFy, 1, fy.length);
      newFx[newFx.length - 1] = xFinalControlVector[1];
      newFy[newFy.length - 1] = yFinalControlVector[1];

      for (int i = 0; i < newFx.length - 1; i++) {
        splines[i] = new CubicHermiteSpline(
            new double[]{newWaypts[i].getX(), newFx[i]},
            new double[]{newWaypts[i + 1].getX(), newFx[i + 1]},
            new double[]{newWaypts[i].getY(), newFy[i]},
            new double[]{newWaypts[i + 1].getY(), newFy[i + 1]}
        );
      }
    } else if (waypoints.length == 1) {
      final var xDeriv = (3 * (end.getTranslation().getX()
          - start.getTranslation().getX())
          - xFinalControlVector[1] - xInitialControlVector[1])
          / 4.0;
      final var yDeriv = (3 * (end.getTranslation().getY()
          - start.getTranslation().getY())
          - yFinalControlVector[1] - yInitialControlVector[1])
          / 4.0;

      double[] midXControlVector = {waypoints[0].getX(), xDeriv};
      double[] midYControlVector = {waypoints[0].getX(), yDeriv};

      splines[0] = new CubicHermiteSpline(xInitialControlVector, midXControlVector,
          yInitialControlVector, midYControlVector);
      splines[1] = new CubicHermiteSpline(midXControlVector, xFinalControlVector,
          midYControlVector, yFinalControlVector);
    } else {
      splines[0] = new CubicHermiteSpline(xInitialControlVector, xFinalControlVector,
          yInitialControlVector, yFinalControlVector);
    }
    return splines;
  }

  /**
   * Returns a set of quintic splines corresponding to the provided waypoints.
   * The user is free to set the direction of all waypoints. Continuous
   * curvature is guaranteed throughout the path.
   *
   * @param waypoints The waypoints.
   * @return A vector of quintic hermite splines that interpolate through the
   *         provided waypoints.
   */
  @SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops"})
  public static QuinticHermiteSpline[] getQuinticSplinesFromWaypoints(Pose2d[] waypoints) {
    QuinticHermiteSpline[] splines = new QuinticHermiteSpline[waypoints.length - 1];
    for (int i = 0; i < waypoints.length - 1; i++) {
      var p0 = waypoints[i];
      var p1 = waypoints[i + 1];

      // This just makes the splines look better.
      final var scalar = 1.2 * p0.getTranslation().getDistance(p1.getTranslation());

      double[] xInitialControlVector =
          {p0.getTranslation().getX(), scalar * p0.getRotation().getCos(), 0.0};
      double[] xFinalControlVector =
          {p1.getTranslation().getX(), scalar * p1.getRotation().getCos(), 0.0};
      double[] yInitialControlVector =
          {p0.getTranslation().getY(), scalar * p0.getRotation().getSin(), 0.0};
      double[] yFinalControlVector =
          {p1.getTranslation().getY(), scalar * p1.getRotation().getSin(), 0.0};

      splines[i] = new QuinticHermiteSpline(xInitialControlVector, xFinalControlVector,
          yInitialControlVector, yFinalControlVector);
    }
    return splines;
  }

  /**
   * Thomas algorithm for solving tridiagonal systems Af = d.
   *
   * @param a              the values of A above the diagonal
   * @param b              the values of A on the diagonal
   * @param c              the values of A below the diagonal
   * @param d              the vector on the rhs
   * @param solutionVector the unknown (solution) vector, modified in-place
   */
  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  private static void thomasAlgorithm(double[] a, double[] b,
                                      double[] c, double[] d, double[] solutionVector) {
    int N = d.length;

    double[] cStar = new double[N];
    double[] dStar = new double[N];

    // This updates the coefficients in the first row
    // Note that we should be checking for division by zero here
    cStar[0] = c[0] / b[0];
    dStar[0] = d[0] / b[0];

    // Create the c_star and d_star coefficients in the forward sweep
    for (int i = 1; i < N; i++) {
      double m = 1.0 / (b[i] - a[i] * cStar[i - 1]);
      cStar[i] = c[i] * m;
      dStar[i] = (d[i] - a[i] * dStar[i - 1]) * m;
    }
    solutionVector[N - 1] = dStar[N - 1];
    // This is the reverse sweep, used to update the solution vector f
    for (int i = N - 2; i >= 0; i--) {
      solutionVector[i] = dStar[i] - cStar[i] * solutionVector[i + 1];
    }
  }
}
