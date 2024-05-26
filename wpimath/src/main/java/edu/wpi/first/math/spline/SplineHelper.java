// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.spline;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Translation2d;
import java.util.Arrays;
import java.util.List;
import org.ejml.simple.SimpleMatrix;

/** Helper class that is used to generate cubic and quintic splines from user provided waypoints. */
public final class SplineHelper {
  /** Private constructor because this is a utility class. */
  private SplineHelper() {}

  /**
   * Returns 2 cubic control vectors from a set of exterior waypoints and interior translations.
   *
   * @param start The starting pose.
   * @param interiorWaypoints The interior waypoints.
   * @param end The ending pose.
   * @return 2 cubic control vectors.
   */
  public static Spline.ControlVector[] getCubicControlVectorsFromWaypoints(
      Pose2d start, Translation2d[] interiorWaypoints, Pose2d end) {
    // Generate control vectors from poses.
    Spline.ControlVector initialCV;
    Spline.ControlVector endCV;

    // Chooses a magnitude automatically that makes the splines look better.
    if (interiorWaypoints.length < 1) {
      double scalar = start.getTranslation().getDistance(end.getTranslation()) * 1.2;
      initialCV = getCubicControlVector(scalar, start);
      endCV = getCubicControlVector(scalar, end);
    } else {
      double scalar = start.getTranslation().getDistance(interiorWaypoints[0]) * 1.2;
      initialCV = getCubicControlVector(scalar, start);
      scalar =
          end.getTranslation().getDistance(interiorWaypoints[interiorWaypoints.length - 1]) * 1.2;
      endCV = getCubicControlVector(scalar, end);
    }
    return new Spline.ControlVector[] {initialCV, endCV};
  }

  /**
   * Returns quintic splines from a set of waypoints.
   *
   * @param waypoints The waypoints
   * @return array of splines.
   */
  public static QuinticHermiteSpline[] getQuinticSplinesFromWaypoints(List<Pose2d> waypoints) {
    QuinticHermiteSpline[] splines = new QuinticHermiteSpline[waypoints.size() - 1];
    for (int i = 0; i < waypoints.size() - 1; ++i) {
      var p0 = waypoints.get(i);
      var p1 = waypoints.get(i + 1);

      // This just makes the splines look better.
      final var scalar = 1.2 * p0.getTranslation().getDistance(p1.getTranslation());

      var controlVecA = getQuinticControlVector(scalar, p0);
      var controlVecB = getQuinticControlVector(scalar, p1);

      splines[i] =
          new QuinticHermiteSpline(controlVecA.x, controlVecB.x, controlVecA.y, controlVecB.y);
    }
    return splines;
  }

  /**
   * Returns a set of cubic splines corresponding to the provided control vectors. The user is free
   * to set the direction of the start and end point. The directions for the middle waypoints are
   * determined automatically to ensure continuous curvature throughout the path.
   *
   * @param start The starting control vector.
   * @param waypoints The middle waypoints. This can be left blank if you only wish to create a path
   *     with two waypoints.
   * @param end The ending control vector.
   * @return A vector of cubic hermite splines that interpolate through the provided waypoints and
   *     control vectors.
   */
  public static CubicHermiteSpline[] getCubicSplinesFromControlVectors(
      Spline.ControlVector start, Translation2d[] waypoints, Spline.ControlVector end) {
    CubicHermiteSpline[] splines = new CubicHermiteSpline[waypoints.length + 1];

    double[] xInitial = start.x;
    double[] yInitial = start.y;
    double[] xFinal = end.x;
    double[] yFinal = end.y;

    if (waypoints.length > 1) {
      Translation2d[] newWaypts = new Translation2d[waypoints.length + 2];

      // Create an array of all waypoints, including the start and end.
      newWaypts[0] = new Translation2d(xInitial[0], yInitial[0]);
      System.arraycopy(waypoints, 0, newWaypts, 1, waypoints.length);
      newWaypts[newWaypts.length - 1] = new Translation2d(xFinal[0], yFinal[0]);

      // Populate tridiagonal system for clamped cubic
      /* See:
      https://www.uio.no/studier/emner/matnat/ifi/nedlagte-emner/INF-MAT4350/h08
      /undervisningsmateriale/chap7alecture.pdf
      */
      // Above-diagonal of tridiagonal matrix, zero-padded
      final double[] a = new double[newWaypts.length - 2];

      // Diagonal of tridiagonal matrix
      final double[] b = new double[newWaypts.length - 2];
      Arrays.fill(b, 4.0);

      // Below-diagonal of tridiagonal matrix, zero-padded
      final double[] c = new double[newWaypts.length - 2];

      // rhs vectors
      final double[] dx = new double[newWaypts.length - 2];
      final double[] dy = new double[newWaypts.length - 2];

      // solution vectors
      final double[] fx = new double[newWaypts.length - 2];
      final double[] fy = new double[newWaypts.length - 2];

      // populate above-diagonal and below-diagonal vectors
      a[0] = 0.0;
      for (int i = 0; i < newWaypts.length - 3; i++) {
        a[i + 1] = 1;
        c[i] = 1;
      }
      c[c.length - 1] = 0.0;

      // populate rhs vectors
      dx[0] = 3 * (newWaypts[2].getX() - newWaypts[0].getX()) - xInitial[1];
      dy[0] = 3 * (newWaypts[2].getY() - newWaypts[0].getY()) - yInitial[1];

      if (newWaypts.length > 4) {
        for (int i = 1; i <= newWaypts.length - 4; i++) {
          // dx and dy represent the derivatives of the internal waypoints. The derivative
          // of the second internal waypoint should involve the third and first internal waypoint,
          // which have indices of 1 and 3 in the newWaypts list (which contains ALL waypoints).
          dx[i] = 3 * (newWaypts[i + 2].getX() - newWaypts[i].getX());
          dy[i] = 3 * (newWaypts[i + 2].getY() - newWaypts[i].getY());
        }
      }

      dx[dx.length - 1] =
          3 * (newWaypts[newWaypts.length - 1].getX() - newWaypts[newWaypts.length - 3].getX())
              - xFinal[1];
      dy[dy.length - 1] =
          3 * (newWaypts[newWaypts.length - 1].getY() - newWaypts[newWaypts.length - 3].getY())
              - yFinal[1];

      // Compute solution to tridiagonal system
      thomasAlgorithm(a, b, c, dx, fx);
      thomasAlgorithm(a, b, c, dy, fy);

      double[] newFx = new double[fx.length + 2];
      double[] newFy = new double[fy.length + 2];

      newFx[0] = xInitial[1];
      newFy[0] = yInitial[1];
      System.arraycopy(fx, 0, newFx, 1, fx.length);
      System.arraycopy(fy, 0, newFy, 1, fy.length);
      newFx[newFx.length - 1] = xFinal[1];
      newFy[newFy.length - 1] = yFinal[1];

      for (int i = 0; i < newFx.length - 1; i++) {
        splines[i] =
            new CubicHermiteSpline(
                new double[] {newWaypts[i].getX(), newFx[i]},
                new double[] {newWaypts[i + 1].getX(), newFx[i + 1]},
                new double[] {newWaypts[i].getY(), newFy[i]},
                new double[] {newWaypts[i + 1].getY(), newFy[i + 1]});
      }
    } else if (waypoints.length == 1) {
      final var xDeriv = (3 * (xFinal[0] - xInitial[0]) - xFinal[1] - xInitial[1]) / 4.0;
      final var yDeriv = (3 * (yFinal[0] - yInitial[0]) - yFinal[1] - yInitial[1]) / 4.0;

      double[] midXControlVector = {waypoints[0].getX(), xDeriv};
      double[] midYControlVector = {waypoints[0].getY(), yDeriv};

      splines[0] =
          new CubicHermiteSpline(
              xInitial, midXControlVector,
              yInitial, midYControlVector);
      splines[1] =
          new CubicHermiteSpline(
              midXControlVector, xFinal,
              midYControlVector, yFinal);
    } else {
      splines[0] =
          new CubicHermiteSpline(
              xInitial, xFinal,
              yInitial, yFinal);
    }
    return splines;
  }

  /**
   * Returns a set of quintic splines corresponding to the provided control vectors. The user is
   * free to set the direction of all control vectors. Continuous curvature is guaranteed throughout
   * the path.
   *
   * @param controlVectors The control vectors.
   * @return A vector of quintic hermite splines that interpolate through the provided waypoints.
   */
  public static QuinticHermiteSpline[] getQuinticSplinesFromControlVectors(
      Spline.ControlVector[] controlVectors) {
    QuinticHermiteSpline[] splines = new QuinticHermiteSpline[controlVectors.length - 1];
    for (int i = 0; i < controlVectors.length - 1; i++) {
      var xInitial = controlVectors[i].x;
      var xFinal = controlVectors[i + 1].x;
      var yInitial = controlVectors[i].y;
      var yFinal = controlVectors[i + 1].y;
      splines[i] =
          new QuinticHermiteSpline(
              xInitial, xFinal,
              yInitial, yFinal);
    }
    return splines;
  }

  /**
   * Optimizes the curvature of 2 or more quintic splines at knot points. Overall, this reduces the
   * integral of the absolute value of the second derivative across the set of splines.
   *
   * @param splines An array of un-optimized quintic splines.
   * @return An array of optimized quintic splines.
   */
  @SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops"})
  public static QuinticHermiteSpline[] optimizeCurvature(QuinticHermiteSpline[] splines) {
    // If there's only spline in the array, we can't optimize anything so just return that.
    if (splines.length < 2) {
      return splines;
    }

    // Implements Section 4.1.2 of
    // http://www2.informatik.uni-freiburg.de/~lau/students/Sprunk2008.pdf.

    // Cubic splines minimize the integral of the second derivative's absolute value. Therefore, we
    // can create cubic splines with the same 0th and 1st derivatives and the provided quintic
    // splines, find the second derivative of those cubic splines and then use a weighted average
    // for the second derivatives of the quintic splines.

    QuinticHermiteSpline[] optimizedSplines = new QuinticHermiteSpline[splines.length];
    for (int i = 0; i < splines.length - 1; ++i) {
      QuinticHermiteSpline a = splines[i];
      QuinticHermiteSpline b = splines[i + 1];

      // Get the control vectors that created the quintic splines above.
      Spline.ControlVector aInitial = a.getInitialControlVector();
      Spline.ControlVector aFinal = a.getFinalControlVector();
      Spline.ControlVector bInitial = b.getInitialControlVector();
      Spline.ControlVector bFinal = b.getFinalControlVector();

      // Create cubic splines with the same control vectors.
      CubicHermiteSpline ca = new CubicHermiteSpline(aInitial.x, aFinal.x, aInitial.y, aFinal.y);
      CubicHermiteSpline cb = new CubicHermiteSpline(bInitial.x, bFinal.x, bInitial.y, bFinal.y);

      // Calculate the second derivatives at the knot points.
      SimpleMatrix bases = new SimpleMatrix(4, 1, true, 1, 1, 1, 1);
      SimpleMatrix combinedA = ca.getCoefficients().mult(bases);

      double ddxA = combinedA.get(4, 0);
      double ddyA = combinedA.get(5, 0);
      double ddxB = cb.getCoefficients().get(4, 1);
      double ddyB = cb.getCoefficients().get(5, 1);

      // Calculate the parameters for the weighted average.
      double dAB = Math.hypot(aFinal.x[0] - aInitial.x[0], aFinal.y[0] - aInitial.y[0]);
      double dBC = Math.hypot(bFinal.x[0] - bInitial.x[0], bFinal.y[0] - bInitial.y[0]);
      double alpha = dBC / (dAB + dBC);
      double beta = dAB / (dAB + dBC);

      // Calculate the weighted average.
      double ddx = alpha * ddxA + beta * ddxB;
      double ddy = alpha * ddyA + beta * ddyB;

      // Create new splines.
      optimizedSplines[i] =
          new QuinticHermiteSpline(
              aInitial.x,
              new double[] {aFinal.x[0], aFinal.x[1], ddx},
              aInitial.y,
              new double[] {aFinal.y[0], aFinal.y[1], ddy});
      optimizedSplines[i + 1] =
          new QuinticHermiteSpline(
              new double[] {bInitial.x[0], bInitial.x[1], ddx},
              bFinal.x,
              new double[] {bInitial.y[0], bInitial.y[1], ddy},
              bFinal.y);
    }
    return optimizedSplines;
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
  private static void thomasAlgorithm(
      double[] a, double[] b, double[] c, double[] d, double[] solutionVector) {
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

  private static Spline.ControlVector getCubicControlVector(double scalar, Pose2d point) {
    return new Spline.ControlVector(
        new double[] {point.getX(), scalar * point.getRotation().getCos()},
        new double[] {point.getY(), scalar * point.getRotation().getSin()});
  }

  private static Spline.ControlVector getQuinticControlVector(double scalar, Pose2d point) {
    return new Spline.ControlVector(
        new double[] {point.getX(), scalar * point.getRotation().getCos(), 0.0},
        new double[] {point.getY(), scalar * point.getRotation().getSin(), 0.0});
  }
}
