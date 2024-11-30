// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.jni.TrajectoryUtilJNI;
import java.io.IOException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

/** Trajectory utilities. */
public final class TrajectoryUtil {
  private TrajectoryUtil() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Creates a trajectory from a double[] of elements.
   *
   * @param elements A double[] containing the raw elements of the trajectory.
   * @return A trajectory created from the elements.
   */
  private static Trajectory createTrajectoryFromElements(double[] elements) {
    // Make sure that the elements have the correct length.
    if (elements.length % 7 != 0) {
      throw new TrajectorySerializationException(
          "An error occurred when converting trajectory elements into a trajectory.");
    }

    // Create a list of states from the elements.
    List<Trajectory.State> states = new ArrayList<>();
    for (int i = 0; i < elements.length; i += 7) {
      states.add(
          new Trajectory.State(
              elements[i],
              elements[i + 1],
              elements[i + 2],
              new Pose2d(elements[i + 3], elements[i + 4], new Rotation2d(elements[i + 5])),
              elements[i + 6]));
    }
    return new Trajectory(states);
  }

  /**
   * Returns a double[] of elements from the given trajectory.
   *
   * @param trajectory The trajectory to retrieve raw elements from.
   * @return A double[] of elements from the given trajectory.
   */
  private static double[] getElementsFromTrajectory(Trajectory trajectory) {
    // Create a double[] of elements and fill it from the trajectory states.
    double[] elements = new double[trajectory.getStates().size() * 7];

    for (int i = 0; i < trajectory.getStates().size() * 7; i += 7) {
      var state = trajectory.getStates().get(i / 7);
      elements[i] = state.timeSeconds;
      elements[i + 1] = state.velocityMetersPerSecond;
      elements[i + 2] = state.accelerationMetersPerSecondSq;
      elements[i + 3] = state.poseMeters.getX();
      elements[i + 4] = state.poseMeters.getY();
      elements[i + 5] = state.poseMeters.getRotation().getRadians();
      elements[i + 6] = state.curvatureRadPerMeter;
    }
    return elements;
  }

  private static int pathWeaverTrajectoryInstances;

  /**
   * Imports a Trajectory from a JSON file exported from PathWeaver.
   *
   * @param path The path of the json file to import from
   * @return The trajectory represented by the file.
   * @throws IOException if reading from the file fails.
   */
  public static Trajectory fromPathweaverJson(Path path) throws IOException {
    MathSharedStore.reportUsage(
        MathUsageId.kTrajectory_PathWeaver, ++pathWeaverTrajectoryInstances);
    return createTrajectoryFromElements(TrajectoryUtilJNI.fromPathweaverJson(path.toString()));
  }

  /**
   * Exports a Trajectory to a PathWeaver-style JSON file.
   *
   * @param trajectory The trajectory to export
   * @param path The path of the file to export to
   * @throws IOException if writing to the file fails.
   */
  public static void toPathweaverJson(Trajectory trajectory, Path path) throws IOException {
    TrajectoryUtilJNI.toPathweaverJson(getElementsFromTrajectory(trajectory), path.toString());
  }

  /**
   * Deserializes a Trajectory from JSON exported from PathWeaver.
   *
   * @param json The string containing the serialized JSON
   * @return the trajectory represented by the JSON
   * @throws TrajectorySerializationException if deserialization of the string fails.
   */
  public static Trajectory deserializeTrajectory(String json) {
    return createTrajectoryFromElements(TrajectoryUtilJNI.deserializeTrajectory(json));
  }

  /**
   * Serializes a Trajectory to PathWeaver-style JSON.
   *
   * @param trajectory The trajectory to export
   * @return The string containing the serialized JSON
   * @throws TrajectorySerializationException if serialization of the trajectory fails.
   */
  public static String serializeTrajectory(Trajectory trajectory) {
    return TrajectoryUtilJNI.serializeTrajectory(getElementsFromTrajectory(trajectory));
  }

  /** Exception for trajectory serialization failure. */
  public static class TrajectorySerializationException extends RuntimeException {
    /**
     * Constructs a TrajectorySerializationException.
     *
     * @param message The exception message.
     */
    public TrajectorySerializationException(String message) {
      super(message);
    }
  }
}
