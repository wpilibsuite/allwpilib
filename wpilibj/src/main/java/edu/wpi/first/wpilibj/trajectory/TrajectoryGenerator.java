/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Transform2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.spline.PoseWithCurvature;
import edu.wpi.first.wpilibj.spline.Spline;
import edu.wpi.first.wpilibj.spline.SplineHelper;
import edu.wpi.first.wpilibj.spline.SplineParameterizer;

public final class TrajectoryGenerator {
  /**
   * Private constructor because this is a utility class.
   */
  private TrajectoryGenerator() {
  }

  /**
   * Generates a trajectory from the given control vectors and config. This method uses clamped
   * cubic splines -- a method in which the exterior control vectors and interior waypoints
   * are provided. The headings are automatically determined at the interior points to
   * ensure continuous curvature.
   *
   * @param initial           The initial control vector.
   * @param interiorWaypoints The interior waypoints.
   * @param end               The ending control vector.
   * @param config            The configuration for the trajectory.
   * @return The generated trajectory.
   */
  public static Trajectory generateTrajectory(
      Spline.ControlVector initial,
      List<Translation2d> interiorWaypoints,
      Spline.ControlVector end,
      TrajectoryConfig config
  ) {
    final var flip = new Transform2d(new Translation2d(), Rotation2d.fromDegrees(180.0));

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
    var points = splinePointsFromSplines(SplineHelper.getCubicSplinesFromControlVectors(
        newInitial, interiorWaypoints.toArray(new Translation2d[0]), newEnd
    ));

    // Change the points back to their original orientation.
    if (config.isReversed()) {
      for (var point : points) {
        point.poseMeters = point.poseMeters.plus(flip);
        point.curvatureRadPerMeter *= -1;
      }
    }

    // Generate and return trajectory.
    return TrajectoryParameterizer.timeParameterizeTrajectory(points, config.getConstraints(),
        config.getStartVelocity(), config.getEndVelocity(), config.getMaxVelocity(),
        config.getMaxAcceleration(), config.isReversed());
  }

  /**
   * Generates a trajectory from the given waypoints and config. This method uses clamped
   * cubic splines -- a method in which the initial pose, final pose, and interior waypoints
   * are provided.  The headings are automatically determined at the interior points to
   * ensure continuous curvature.
   *
   * @param start             The starting pose.
   * @param interiorWaypoints The interior waypoints.
   * @param end               The ending pose.
   * @param config            The configuration for the trajectory.
   * @return The generated trajectory.
   */
  public static Trajectory generateTrajectory(
      Pose2d start, List<Translation2d> interiorWaypoints, Pose2d end,
      TrajectoryConfig config
  ) {
    var controlVectors = SplineHelper.getCubicControlVectorsFromWaypoints(
        start, interiorWaypoints.toArray(new Translation2d[0]), end
    );

    // Return the generated trajectory.
    return generateTrajectory(controlVectors[0], interiorWaypoints, controlVectors[1], config);
  }

  /**
   * Generates a trajectory from the given quintic control vectors and config. This method
   * uses quintic hermite splines -- therefore, all points must be represented by control
   * vectors. Continuous curvature is guaranteed in this method.
   *
   * @param controlVectors List of quintic control vectors.
   * @param config         The configuration for the trajectory.
   * @return The generated trajectory.
   */
  @SuppressWarnings("PMD.AvoidInstantiatingObjectsInLoops")
  public static Trajectory generateTrajectory(
      ControlVectorList controlVectors,
      TrajectoryConfig config
  ) {
    final var flip = new Transform2d(new Translation2d(), Rotation2d.fromDegrees(180.0));
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
    var points = splinePointsFromSplines(SplineHelper.getQuinticSplinesFromControlVectors(
        newControlVectors.toArray(new Spline.ControlVector[]{})
    ));

    // Change the points back to their original orientation.
    if (config.isReversed()) {
      for (var point : points) {
        point.poseMeters = point.poseMeters.plus(flip);
        point.curvatureRadPerMeter *= -1;
      }
    }

    // Generate and return trajectory.
    return TrajectoryParameterizer.timeParameterizeTrajectory(points, config.getConstraints(),
        config.getStartVelocity(), config.getEndVelocity(), config.getMaxVelocity(),
        config.getMaxAcceleration(), config.isReversed());

  }

  /**
   * Generates a trajectory from the given waypoints and config. This method
   * uses quintic hermite splines -- therefore, all points must be represented by Pose2d
   * objects. Continuous curvature is guaranteed in this method.
   *
   * @param waypoints List of waypoints..
   * @param config    The configuration for the trajectory.
   * @return The generated trajectory.
   */
  @SuppressWarnings("LocalVariableName")
  public static Trajectory generateTrajectory(List<Pose2d> waypoints, TrajectoryConfig config) {
    var originalList = SplineHelper.getQuinticControlVectorsFromWaypoints(waypoints);
    var newList = new ControlVectorList(originalList);
    return generateTrajectory(newList, config);
  }

  /**
   * Generate spline points from a vector of splines by parameterizing the
   * splines.
   *
   * @param splines The splines to parameterize.
   * @return The spline points for use in time parameterization of a trajectory.
   */
  public static List<PoseWithCurvature> splinePointsFromSplines(
      Spline[] splines) {
    // Create the vector of spline points.
    var splinePoints = new ArrayList<PoseWithCurvature>();

    // Add the first point to the vector.
    splinePoints.add(splines[0].getPoint(0.0));

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

  // Work around type erasure signatures
  public static class ControlVectorList extends ArrayList<Spline.ControlVector> {
    public ControlVectorList(int initialCapacity) {
      super(initialCapacity);
    }

    public ControlVectorList() {
      super();
    }

    public ControlVectorList(Collection<? extends Spline.ControlVector> collection) {
      super(collection);
    }
  }
}
