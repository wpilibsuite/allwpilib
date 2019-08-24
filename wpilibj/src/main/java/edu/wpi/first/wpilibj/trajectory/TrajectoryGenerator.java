/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory;

import java.util.ArrayList;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Transform2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.spline.PoseWithCurvature;
import edu.wpi.first.wpilibj.spline.Spline;
import edu.wpi.first.wpilibj.spline.SplineHelper;
import edu.wpi.first.wpilibj.trajectory.constraint.TrajectoryConstraint;

public class TrajectoryGenerator {
  public static Trajectory generateTrajectory(
      ArrayList<Pose2d> waypoints,
      ArrayList<TrajectoryConstraint> constraints,
      double startVelocityMetersPerSecond,
      double endVelocityMetersPerSecond,
      double maxVelocityMetersPerSecond,
      double maxAccelerationMetersPerSecondSq,
      boolean reversed
  ) {
    final var flip = new Transform2d(new Translation2d(), Rotation2d.fromDegrees(-180.0));

    // Make theta normal for trajectory generation if path is reversed.
    final var newWaypoints = new ArrayList<Pose2d>(waypoints.size());
    for (final var point : waypoints) {
      newWaypoints.add(reversed ? point.plus(flip) : point);
    }

    var points = splinePointsFromSplines(SplineHelper.getQuinticSplinesFromWaypoints(
        newWaypoints.toArray(new Pose2d[0])
    ));

    // After trajectory generation, flip theta back so it's relative to the
    // field. Also fix curvature.
    if (reversed) {
      for (var point : points) {
        point.pose = point.pose.plus(flip);
        point.curvature *= -1;
      }
    }

    return timeParameterizeTrajectory(points, constraints,
        startVelocityMetersPerSecond, endVelocityMetersPerSecond, maxVelocityMetersPerSecond,
        maxAccelerationMetersPerSecondSq, reversed);
  }

  public static Trajectory generateTrajectory(
      Pose2d start,
      ArrayList<Translation2d> waypoints,
      Pose2d end,
      ArrayList<TrajectoryConstraint> constraints,
      double startVelocityMetersPerSecond,
      double endVelocityMetersPerSecond,
      double maxVelocityMetersPerSecond,
      double maxAccelerationMetersPerSecondSq,
      boolean reversed
  ) {
    final var flip = new Transform2d(new Translation2d(), Rotation2d.fromDegrees(-180.0));

    final var newStart = reversed ? start.plus(flip) : start;
    final var newEnd = reversed ? end.plus(flip) : end;

    var points = splinePointsFromSplines(SplineHelper.getCubicSplinesFromWaypoints(
        newStart, waypoints.toArray(new Translation2d[0]), newEnd
    ));

    // After trajectory generation, flip theta back so it's relative to the
    // field. Also fix curvature.
    if (reversed) {
      for (var point : points) {
        point.pose = point.pose.plus(flip);
        point.curvature *= -1;
      }
    }

    return timeParameterizeTrajectory(points, constraints,
        startVelocityMetersPerSecond, endVelocityMetersPerSecond, maxVelocityMetersPerSecond,
        maxAccelerationMetersPerSecondSq, reversed);
  }

  private static ArrayList<PoseWithCurvature> splinePointsFromSplines(
      Spline[] splines) {
    // Create the vector of spline points.
    var splinePoints = new ArrayList<PoseWithCurvature>();

    // Add the first point to the vector.
    splinePoints.add(splines[0].getPoint(0.0));

    // Iterate through the vector and parameterize each spline, adding the
    // parameterized points to the final vector.
    for (final var spline : splines) {
      var points = spline.parameterize();

      // Append the array of poses to the vector. We are removing the first
      // point because it's a duplicate of the last point from the previous
      // spline.
      splinePoints.addAll(points.subList(1, points.size()));
    }
    return splinePoints;
  }

  private static Trajectory timeParameterizeTrajectory(
      ArrayList<PoseWithCurvature> points,
      ArrayList<TrajectoryConstraint> constraints,
      double startVelocityMetersPerSecond,
      double endVelocityMetersPerSecond,
      double maxVelocityMetersPerSecond,
      double maxAccelerationMetersPerSecondSq,
      boolean reversed
  ) {
    var constrainedStates = new ArrayList<ConstrainedState>(points.size());
    var predecessor = new ConstrainedState(points.get(0), 0, startVelocityMetersPerSecond,
        -maxAccelerationMetersPerSecondSq, maxAccelerationMetersPerSecondSq);

    // Forward pass
    for (int i = 0; i < points.size(); i++) {
      constrainedStates.add(new ConstrainedState());
      var constrainedState = constrainedStates.get(i);
      constrainedState.pose = points.get(i);

      // Begin constraining based on predecessor.
      double ds = constrainedState.pose.pose.getTranslation().getDistance(
          predecessor.pose.pose.getTranslation());
      constrainedState.distanceMeters = predecessor.distanceMeters + ds;

      // We may need to iterate to find the maximum end velocity and common
      // acceleration, since acceleration limits may be a function of velocity.
      while (true) {
        // Enforce global max velocity and max reachable velocity by global
        // acceleration limit. vf = std::sqrt(vi^2 + 2*a*d).
        constrainedState.maxVelocityMetersPerSecond = Math.min(
            maxVelocityMetersPerSecond,
            Math.sqrt(predecessor.maxVelocityMetersPerSecond
                * predecessor.maxVelocityMetersPerSecond
                + predecessor.maxAccelerationMetersPerSecondSq * ds * 2.0)
        );

        constrainedState.minAccelerationMetersPerSecondSq = -maxAccelerationMetersPerSecondSq;
        constrainedState.maxAccelerationMetersPerSecondSq = maxAccelerationMetersPerSecondSq;

        // At this point, the constrained state is fully constructed apart from
        // all the custom-defined user constraints.
        for (final var constraint : constraints) {
          constrainedState.maxVelocityMetersPerSecond = Math.min(
              constrainedState.maxVelocityMetersPerSecond,
              constraint.getMaxVelocityMetersPerSecond(
                  constrainedState.pose.pose, constrainedState.pose.curvature)
          );
        }

        if (Double.isNaN(constrainedState.maxVelocityMetersPerSecond)) {
          throw new RuntimeException();
        }

        // Now enforce all acceleration limits.
        enforceAccelerationLimits(reversed, constraints, constrainedState);

        if (ds < 1E-6) {
          break;
        }

        // If the actual acceleration for this state is higher than the max
        // acceleration that we applied, then we need to reduce the max
        // acceleration of the predecessor and try again.
        double actualAcceleration = (constrainedState.maxVelocityMetersPerSecond
            * constrainedState.maxVelocityMetersPerSecond
            - predecessor.maxVelocityMetersPerSecond * predecessor.maxVelocityMetersPerSecond)
            / (ds * 2.0);

        // If we violate the max acceleration constraint, let's modify the
        // predecessor.
        if (constrainedState.maxAccelerationMetersPerSecondSq < actualAcceleration - 1E-6) {
          predecessor.maxAccelerationMetersPerSecondSq
              = constrainedState.maxAccelerationMetersPerSecondSq;
        } else {
          // Constrain the predecessor's max acceleration to the current
          // acceleration.
          if (actualAcceleration > predecessor.minAccelerationMetersPerSecondSq) {
            predecessor.maxAccelerationMetersPerSecondSq = actualAcceleration;
          }
          // If the actual acceleration is less than the predecessor's min
          // acceleration, it will be repaired in the backward pass.
          break;
        }
      }
      predecessor = constrainedState;
    }

    var successor = new ConstrainedState(points.get(points.size() - 1),
        constrainedStates.get(constrainedStates.size() - 1).distanceMeters,
        endVelocityMetersPerSecond,
        -maxAccelerationMetersPerSecondSq, maxAccelerationMetersPerSecondSq);

    // Backward pass
    for (int i = points.size() - 1; i >= 0; i--) {
      var constrainedState = constrainedStates.get(i);
      double ds = constrainedState.distanceMeters - successor.distanceMeters; // negative

      while (true) {
        // Enforce max velocity limit (reverse)
        // vf = std::sqrt(vi^2 + 2*a*d), where vi = successor.
        double newMaxVelocity = Math.sqrt(
            successor.maxVelocityMetersPerSecond * successor.maxVelocityMetersPerSecond
                + successor.minAccelerationMetersPerSecondSq * ds * 2.0
        );

        // No more limits to impose! This state can be finalized.
        if (newMaxVelocity >= constrainedState.maxVelocityMetersPerSecond) {
          break;
        }

        constrainedState.maxVelocityMetersPerSecond = newMaxVelocity;

        // Check all acceleration constraints with the new max velocity.
        enforceAccelerationLimits(reversed, constraints, constrainedState);

        if (ds > -1E-6) {
          break;
        }

        // If the actual acceleration for this state is lower than the min
        // acceleration, then we need to lower the min acceleration of the
        // successor and try again.
        double actualAcceleration = (constrainedState.maxVelocityMetersPerSecond
            * constrainedState.maxVelocityMetersPerSecond
            - successor.maxVelocityMetersPerSecond * successor.maxVelocityMetersPerSecond)
            / (ds * 2.0);

        if (constrainedState.minAccelerationMetersPerSecondSq > actualAcceleration + 1E-6) {
          successor.minAccelerationMetersPerSecondSq
              = constrainedState.minAccelerationMetersPerSecondSq;
        } else {
          successor.minAccelerationMetersPerSecondSq = actualAcceleration;
          break;
        }
      }
      successor = constrainedState;
    }

    // Now we can integrate the constrained states forward in time to obtain our
    // trajectory states.
    var states = new ArrayList<Trajectory.State>(points.size());
    double timeSeconds = 0.0;
    double distanceMeters = 0.0;
    double velocityMetersPerSecond = 0.0;

    for (int i = 0; i < constrainedStates.size(); i++) {
      final var state = constrainedStates.get(i);

      // Calculate the change in position between the current state and the previous
      // state.
      double ds = state.distanceMeters - distanceMeters;

      // Calculate the acceleration between the current state and the previous
      // state.
      double accel = (state.maxVelocityMetersPerSecond * state.maxVelocityMetersPerSecond
          - velocityMetersPerSecond * velocityMetersPerSecond) / (ds * 2);

      // Calculate dt
      double dt = 0.0;
      if (i > 0) {
        states.get(i - 1).accelerationMetersPerSecondSq = reversed ? -accel : accel;
        if (Math.abs(accel) > 1E-6) {
          // v_f = v_0 + a * t
          dt = (state.maxVelocityMetersPerSecond - velocityMetersPerSecond) / accel;
        } else if (Math.abs(velocityMetersPerSecond) > 1E-6) {
          // delta_x = v * t
          dt = ds / velocityMetersPerSecond;
        } else {
          throw new RuntimeException("Something went wrong");
        }
      }

      velocityMetersPerSecond = state.maxVelocityMetersPerSecond;
      distanceMeters = state.distanceMeters;

      timeSeconds += dt;

      states.add(new Trajectory.State(
          timeSeconds,
          reversed ? -velocityMetersPerSecond : velocityMetersPerSecond,
          reversed ? -accel : accel,
          state.pose.pose, state.pose.curvature
      ));
    }

    return new Trajectory(states);
  }

  private static void enforceAccelerationLimits(boolean reverse,
                                                ArrayList<TrajectoryConstraint> constraints,
                                                ConstrainedState state) {

    for (final var constraint : constraints) {
      double factor = reverse ? -1.0 : 1.0;
      final var minMaxAccel = constraint.getMinMaxAccelerationMetersPerSecondSq(state.pose.pose,
          state.pose.curvature, state.maxVelocityMetersPerSecond * factor);

      state.minAccelerationMetersPerSecondSq = Math.max(state.minAccelerationMetersPerSecondSq,
          reverse ? -minMaxAccel.maxAccelerationMetersPerSecondSq
              : minMaxAccel.minAccelerationMetersPerSecondSq);

      state.maxAccelerationMetersPerSecondSq = Math.min(state.maxAccelerationMetersPerSecondSq,
          reverse ? -minMaxAccel.minAccelerationMetersPerSecondSq
              : minMaxAccel.maxAccelerationMetersPerSecondSq);
    }

  }

  @SuppressWarnings("MemberName")
  private static class ConstrainedState {
    PoseWithCurvature pose;
    double distanceMeters;
    double maxVelocityMetersPerSecond;
    double minAccelerationMetersPerSecondSq;
    double maxAccelerationMetersPerSecondSq;

    ConstrainedState(PoseWithCurvature pose, double distanceMeters,
                     double maxVelocityMetersPerSecond,
                     double minAccelerationMetersPerSecondSq,
                     double maxAccelerationMetersPerSecondSq) {
      this.pose = pose;
      this.distanceMeters = distanceMeters;
      this.maxVelocityMetersPerSecond = maxVelocityMetersPerSecond;
      this.minAccelerationMetersPerSecondSq = minAccelerationMetersPerSecondSq;
      this.maxAccelerationMetersPerSecondSq = maxAccelerationMetersPerSecondSq;
    }

    ConstrainedState() {
      pose = new PoseWithCurvature();
    }
  }
}
