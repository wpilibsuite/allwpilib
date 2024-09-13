// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.spline.PoseWithCurvature;
import edu.wpi.first.math.spline.Spline;
import edu.wpi.first.math.spline.SplineHelper;
import edu.wpi.first.math.spline.SplineParameterizer;
import edu.wpi.first.math.spline.SplineParameterizer.MalformedSplineException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.function.BiConsumer;

/** Helper class used to generate trajectories with various constraints. */
public final class TrajectoryGenerator {
  private static final Transform2d kFlip = new Transform2d(Translation2d.kZero, Rotation2d.kPi);

  private static final Trajectory kDoNothingTrajectory =
      new Trajectory(List.of(new Trajectory.State()));
  private static BiConsumer<String, StackTraceElement[]> errorFunc;

  /** Private constructor because this is a utility class. */
  private TrajectoryGenerator() {}

  private static void reportError(String error, StackTraceElement[] stackTrace) {
    if (errorFunc != null) {
      errorFunc.accept(error, stackTrace);
    } else {
      MathSharedStore.reportError(error, stackTrace);
    }
  }

  /**
   * Set error reporting function. By default, DriverStation.reportError() is used.
   *
   * @param func Error reporting function, arguments are error and stackTrace.
   */
  public static void setErrorHandler(BiConsumer<String, StackTraceElement[]> func) {
    errorFunc = func;
  }

  /**
   * Generates a trajectory from the given control vectors and config. This method uses clamped
   * cubic splines -- a method in which the exterior control vectors and interior waypoints are
   * provided. The headings are automatically determined at the interior points to ensure continuous
   * curvature.
   *
   * @param initial The initial control vector.
   * @param interiorWaypoints The interior waypoints.
   * @param end The ending control vector.
   * @param config The configuration for the trajectory.
   * @return The generated trajectory.
   */
  public static Trajectory generateTrajectory(
      Spline.ControlVector initial,
      List<Translation2d> interiorWaypoints,
      Spline.ControlVector end,
      TrajectoryConfig config) {
    // Clone the control vectors.
    var newInitial = new Spline.ControlVector(initial.x, initial.y);
    var newEnd = new Spline.ControlVector(end.x, end.y);

    // Change the orientation if reversed.
    if (config.isReversed()) {
      newInitial.x[1] *= -1;
      newInitial.y[1] *= -1;
      newEnd.x[1] *= -1;
      newEnd.y[1] *= -1;
    }

    // Get the spline points
    List<PoseWithCurvature> points;
    try {
      points =
          splinePointsFromSplines(
              SplineHelper.getCubicSplinesFromControlVectors(
                  newInitial, interiorWaypoints.toArray(new Translation2d[0]), newEnd));
    } catch (MalformedSplineException ex) {
      reportError(ex.getMessage(), ex.getStackTrace());
      return kDoNothingTrajectory;
    }

    // Change the points back to their original orientation.
    if (config.isReversed()) {
      for (var point : points) {
        point.poseMeters = point.poseMeters.plus(kFlip);
        point.curvatureRadPerMeter *= -1;
      }
    }

    // Generate and return trajectory.
    return TrajectoryParameterizer.timeParameterizeTrajectory(
        points,
        config.getConstraints(),
        config.getStartVelocity(),
        config.getEndVelocity(),
        config.getMaxVelocity(),
        config.getMaxAcceleration(),
        config.isReversed());
  }

  /**
   * Generates a trajectory from the given waypoints and config. This method uses clamped cubic
   * splines -- a method in which the initial pose, final pose, and interior waypoints are provided.
   * The headings are automatically determined at the interior points to ensure continuous
   * curvature.
   *
   * @param start The starting pose.
   * @param interiorWaypoints The interior waypoints.
   * @param end The ending pose.
   * @param config The configuration for the trajectory.
   * @return The generated trajectory.
   */
  public static Trajectory generateTrajectory(
      Pose2d start, List<Translation2d> interiorWaypoints, Pose2d end, TrajectoryConfig config) {
    var controlVectors =
        SplineHelper.getCubicControlVectorsFromWaypoints(
            start, interiorWaypoints.toArray(new Translation2d[0]), end);

    // Return the generated trajectory.
    return generateTrajectory(controlVectors[0], interiorWaypoints, controlVectors[1], config);
  }

  /**
   * Generates a trajectory from the given quintic control vectors and config. This method uses
   * quintic hermite splines -- therefore, all points must be represented by control vectors.
   * Continuous curvature is guaranteed in this method.
   *
   * @param controlVectors List of quintic control vectors.
   * @param config The configuration for the trajectory.
   * @return The generated trajectory.
   */
  public static Trajectory generateTrajectory(
      ControlVectorList controlVectors, TrajectoryConfig config) {
    final var newControlVectors = new ArrayList<Spline.ControlVector>(controlVectors.size());

    // Create a new control vector list, flipping the orientation if reversed.
    for (final var vector : controlVectors) {
      var newVector = new Spline.ControlVector(vector.x, vector.y);
      if (config.isReversed()) {
        newVector.x[1] *= -1;
        newVector.y[1] *= -1;
      }
      newControlVectors.add(newVector);
    }

    // Get the spline points
    List<PoseWithCurvature> points;
    try {
      points =
          splinePointsFromSplines(
              SplineHelper.getQuinticSplinesFromControlVectors(
                  newControlVectors.toArray(new Spline.ControlVector[] {})));
    } catch (MalformedSplineException ex) {
      reportError(ex.getMessage(), ex.getStackTrace());
      return kDoNothingTrajectory;
    }

    // Change the points back to their original orientation.
    if (config.isReversed()) {
      for (var point : points) {
        point.poseMeters = point.poseMeters.plus(kFlip);
        point.curvatureRadPerMeter *= -1;
      }
    }

    // Generate and return trajectory.
    return TrajectoryParameterizer.timeParameterizeTrajectory(
        points,
        config.getConstraints(),
        config.getStartVelocity(),
        config.getEndVelocity(),
        config.getMaxVelocity(),
        config.getMaxAcceleration(),
        config.isReversed());
  }

  /**
   * Generates a trajectory from the given waypoints and config. This method uses quintic hermite
   * splines -- therefore, all points must be represented by Pose2d objects. Continuous curvature is
   * guaranteed in this method.
   *
   * @param waypoints List of waypoints..
   * @param config The configuration for the trajectory.
   * @return The generated trajectory.
   */
  public static Trajectory generateTrajectory(List<Pose2d> waypoints, TrajectoryConfig config) {
    List<Pose2d> newWaypoints = new ArrayList<>();
    if (config.isReversed()) {
      for (Pose2d originalWaypoint : waypoints) {
        newWaypoints.add(originalWaypoint.plus(kFlip));
      }
    } else {
      newWaypoints.addAll(waypoints);
    }

    // Get the spline points
    List<PoseWithCurvature> points;
    try {
      points =
          splinePointsFromSplines(
              SplineHelper.optimizeCurvature(
                  SplineHelper.getQuinticSplinesFromWaypoints(newWaypoints)));
    } catch (MalformedSplineException ex) {
      reportError(ex.getMessage(), ex.getStackTrace());
      return kDoNothingTrajectory;
    }

    // Change the points back to their original orientation.
    if (config.isReversed()) {
      for (var point : points) {
        point.poseMeters = point.poseMeters.plus(kFlip);
        point.curvatureRadPerMeter *= -1;
      }
    }

    // Generate and return trajectory.
    return TrajectoryParameterizer.timeParameterizeTrajectory(
        points,
        config.getConstraints(),
        config.getStartVelocity(),
        config.getEndVelocity(),
        config.getMaxVelocity(),
        config.getMaxAcceleration(),
        config.isReversed());
  }

  /**
   * Generate spline points from a vector of splines by parameterizing the splines.
   *
   * @param splines The splines to parameterize.
   * @return The spline points for use in time parameterization of a trajectory.
   * @throws MalformedSplineException When the spline is malformed (e.g. has close adjacent points
   *     with approximately opposing headings)
   */
  public static List<PoseWithCurvature> splinePointsFromSplines(Spline[] splines) {
    // Create the vector of spline points.
    var splinePoints = new ArrayList<PoseWithCurvature>();

    // Add the first point to the vector.
    splinePoints.add(splines[0].getPoint(0.0).get());

    // Iterate through the vector and parameterize each spline, adding the
    // parameterized points to the final vector.
    for (final var spline : splines) {
      var points = SplineParameterizer.parameterize(spline);

      // Append the array of poses to the vector. We are removing the first
      // point because it's a duplicate of the last point from the previous
      // spline.
      splinePoints.addAll(points.subList(1, points.size()));
    }
    return splinePoints;
  }

  /** Control vector list type that works around type erasure signatures. */
  public static class ControlVectorList extends ArrayList<Spline.ControlVector> {
    /** Default constructor. */
    public ControlVectorList() {
      super();
    }

    /**
     * Constructs a ControlVectorList.
     *
     * @param initialCapacity The initial list capacity.
     */
    public ControlVectorList(int initialCapacity) {
      super(initialCapacity);
    }

    /**
     * Constructs a ControlVectorList.
     *
     * @param collection A collection of spline control vectors.
     */
    public ControlVectorList(Collection<? extends Spline.ControlVector> collection) {
      super(collection);
    }
  }
}
