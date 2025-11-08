// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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

package org.wpilib.math.trajectory;

import org.wpilib.math.spline.PoseWithCurvature;
import org.wpilib.math.trajectory.constraint.TrajectoryConstraint;
import java.util.ArrayList;
import java.util.List;

/** Class used to parameterize a trajectory by time. */
public final class TrajectoryParameterizer {
  /** Private constructor because this is a utility class. */
  private TrajectoryParameterizer() {}

  /**
   * Parameterize the trajectory by time. This is where the velocity profile is generated.
   *
   * <p>The derivation of the algorithm used can be found <a
   * href="http://www2.informatik.uni-freiburg.de/~lau/students/Sprunk2008.pdf">here</a>.
   *
   * @param points Reference to the spline points.
   * @param constraints A vector of various velocity and acceleration. constraints.
   * @param startVelocity The start velocity for the trajectory in m/s.
   * @param endVelocity The end velocity for the trajectory in m/s.
   * @param maxVelocity The max velocity for the trajectory in m/s.
   * @param maxAcceleration The max acceleration for the trajectory in m/s².
   * @param reversed Whether the robot should move backwards. Note that the robot will still move
   *     from a -&gt; b -&gt; ... -&gt; z as defined in the waypoints.
   * @return The trajectory.
   */
  public static Trajectory timeParameterizeTrajectory(
      List<PoseWithCurvature> points,
      List<TrajectoryConstraint> constraints,
      double startVelocity,
      double endVelocity,
      double maxVelocity,
      double maxAcceleration,
      boolean reversed) {
    var constrainedStates = new ArrayList<ConstrainedState>(points.size());
    var predecessor =
        new ConstrainedState(points.get(0), 0, startVelocity, -maxAcceleration, maxAcceleration);

    // Forward pass
    for (int i = 0; i < points.size(); i++) {
      constrainedStates.add(new ConstrainedState());
      var constrainedState = constrainedStates.get(i);
      constrainedState.pose = points.get(i);

      // Begin constraining based on predecessor.
      double ds =
          constrainedState
              .pose
              .pose
              .getTranslation()
              .getDistance(predecessor.pose.pose.getTranslation());
      constrainedState.distance = predecessor.distance + ds;

      // We may need to iterate to find the maximum end velocity and common
      // acceleration, since acceleration limits may be a function of velocity.
      while (true) {
        // Enforce global max velocity and max reachable velocity by global
        // acceleration limit. v_f = √(v_i² + 2ad).
        constrainedState.maxVelocity =
            Math.min(
                maxVelocity,
                Math.sqrt(
                    predecessor.maxVelocity * predecessor.maxVelocity
                        + predecessor.maxAcceleration * ds * 2.0));

        constrainedState.minAcceleration = -maxAcceleration;
        constrainedState.maxAcceleration = maxAcceleration;

        // At this point, the constrained state is fully constructed apart from
        // all the custom-defined user constraints.
        for (final var constraint : constraints) {
          constrainedState.maxVelocity =
              Math.min(
                  constrainedState.maxVelocity,
                  constraint.getMaxVelocity(
                      constrainedState.pose.pose,
                      constrainedState.pose.curvature,
                      constrainedState.maxVelocity));
        }

        // Now enforce all acceleration limits.
        enforceAccelerationLimits(reversed, constraints, constrainedState);

        if (ds < 1E-6) {
          break;
        }

        // If the actual acceleration for this state is higher than the max
        // acceleration that we applied, then we need to reduce the max
        // acceleration of the predecessor and try again.
        double actualAcceleration =
            (constrainedState.maxVelocity * constrainedState.maxVelocity
                    - predecessor.maxVelocity * predecessor.maxVelocity)
                / (ds * 2.0);

        // If we violate the max acceleration constraint, let's modify the
        // predecessor.
        if (constrainedState.maxAcceleration < actualAcceleration - 1E-6) {
          predecessor.maxAcceleration = constrainedState.maxAcceleration;
        } else {
          // Constrain the predecessor's max acceleration to the current
          // acceleration.
          if (actualAcceleration > predecessor.minAcceleration) {
            predecessor.maxAcceleration = actualAcceleration;
          }
          // If the actual acceleration is less than the predecessor's min
          // acceleration, it will be repaired in the backward pass.
          break;
        }
      }
      predecessor = constrainedState;
    }

    var successor =
        new ConstrainedState(
            points.get(points.size() - 1),
            constrainedStates.get(constrainedStates.size() - 1).distance,
            endVelocity,
            -maxAcceleration,
            maxAcceleration);

    // Backward pass
    for (int i = points.size() - 1; i >= 0; i--) {
      var constrainedState = constrainedStates.get(i);
      double ds = constrainedState.distance - successor.distance; // negative

      while (true) {
        // Enforce max velocity limit (reverse)
        // v_f = √(v_i² + 2ad), where v_i = successor.
        double newMaxVelocity =
            Math.sqrt(
                successor.maxVelocity * successor.maxVelocity
                    + successor.minAcceleration * ds * 2.0);

        // No more limits to impose! This state can be finalized.
        if (newMaxVelocity >= constrainedState.maxVelocity) {
          break;
        }

        constrainedState.maxVelocity = newMaxVelocity;

        // Check all acceleration constraints with the new max velocity.
        enforceAccelerationLimits(reversed, constraints, constrainedState);

        if (ds > -1E-6) {
          break;
        }

        // If the actual acceleration for this state is lower than the min
        // acceleration, then we need to lower the min acceleration of the
        // successor and try again.
        double actualAcceleration =
            (constrainedState.maxVelocity * constrainedState.maxVelocity
                    - successor.maxVelocity * successor.maxVelocity)
                / (ds * 2.0);

        if (constrainedState.minAcceleration > actualAcceleration + 1E-6) {
          successor.minAcceleration = constrainedState.minAcceleration;
        } else {
          successor.minAcceleration = actualAcceleration;
          break;
        }
      }
      successor = constrainedState;
    }

    // Now we can integrate the constrained states forward in time to obtain our
    // trajectory states.
    var states = new ArrayList<Trajectory.State>(points.size());
    double time = 0.0;
    double distance = 0.0;
    double velocity = 0.0;

    for (int i = 0; i < constrainedStates.size(); i++) {
      final var state = constrainedStates.get(i);

      // Calculate the change in position between the current state and the previous
      // state.
      double ds = state.distance - distance;

      // Calculate the acceleration between the current state and the previous
      // state.
      double accel = (state.maxVelocity * state.maxVelocity - velocity * velocity) / (ds * 2);

      // Calculate dt
      double dt = 0.0;
      if (i > 0) {
        states.get(i - 1).acceleration = reversed ? -accel : accel;
        if (Math.abs(accel) > 1E-6) {
          // v_f = v_0 + a * t
          dt = (state.maxVelocity - velocity) / accel;
        } else if (Math.abs(velocity) > 1E-6) {
          // delta_x = v * t
          dt = ds / velocity;
        } else {
          throw new TrajectoryGenerationException(
              "Something went wrong at iteration " + i + " of time parameterization.");
        }
      }

      velocity = state.maxVelocity;
      distance = state.distance;

      time += dt;

      states.add(
          new Trajectory.State(
              time,
              reversed ? -velocity : velocity,
              reversed ? -accel : accel,
              state.pose.pose,
              state.pose.curvature));
    }

    return new Trajectory(states);
  }

  private static void enforceAccelerationLimits(
      boolean reverse, List<TrajectoryConstraint> constraints, ConstrainedState state) {
    for (final var constraint : constraints) {
      double factor = reverse ? -1.0 : 1.0;
      final var minMaxAccel =
          constraint.getMinMaxAcceleration(
              state.pose.pose, state.pose.curvature, state.maxVelocity * factor);

      if (minMaxAccel.minAcceleration > minMaxAccel.maxAcceleration) {
        throw new TrajectoryGenerationException(
            "Infeasible trajectory constraint: " + constraint.getClass().getName() + "\n");
      }

      state.minAcceleration =
          Math.max(
              state.minAcceleration,
              reverse ? -minMaxAccel.maxAcceleration : minMaxAccel.minAcceleration);

      state.maxAcceleration =
          Math.min(
              state.maxAcceleration,
              reverse ? -minMaxAccel.minAcceleration : minMaxAccel.maxAcceleration);
    }
  }

  private static class ConstrainedState {
    PoseWithCurvature pose;
    double distance;
    double maxVelocity;
    double minAcceleration;
    double maxAcceleration;

    ConstrainedState(
        PoseWithCurvature pose,
        double distance,
        double maxVelocity,
        double minAcceleration,
        double maxAcceleration) {
      this.pose = pose;
      this.distance = distance;
      this.maxVelocity = maxVelocity;
      this.minAcceleration = minAcceleration;
      this.maxAcceleration = maxAcceleration;
    }

    ConstrainedState() {
      pose = new PoseWithCurvature();
    }
  }

  /** Exception for trajectory generation failure. */
  public static class TrajectoryGenerationException extends RuntimeException {
    /**
     * Constructs a TrajectoryGenerationException.
     *
     * @param message Exception message.
     */
    public TrajectoryGenerationException(String message) {
      super(message);
    }
  }
}
