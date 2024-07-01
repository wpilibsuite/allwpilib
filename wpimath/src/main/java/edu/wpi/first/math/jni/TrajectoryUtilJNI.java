// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.jni;

import java.io.IOException;

/** TrajectoryUtil JNI. */
public final class TrajectoryUtilJNI extends WPIMathJNI {
  /**
   * Loads a Pathweaver JSON.
   *
   * @param path The path to the JSON.
   * @return A double array with the trajectory states from the JSON.
   * @throws IOException if the JSON could not be read.
   */
  public static native double[] fromPathweaverJson(String path) throws IOException;

  /**
   * Converts a trajectory into a Pathweaver JSON and saves it.
   *
   * @param elements The elements of the trajectory.
   * @param path The location to save the JSON to.
   * @throws IOException if the JSON could not be written.
   */
  public static native void toPathweaverJson(double[] elements, String path) throws IOException;

  /**
   * Deserializes a trajectory JSON into a double[] of trajectory elements.
   *
   * @param json The JSON containing the serialized trajectory.
   * @return A double array with the trajectory states.
   */
  public static native double[] deserializeTrajectory(String json);

  /**
   * Serializes the trajectory into a JSON string.
   *
   * @param elements The elements of the trajectory.
   * @return A JSON containing the serialized trajectory.
   */
  public static native String serializeTrajectory(double[] elements);

  /** Utility class. */
  private TrajectoryUtilJNI() {}
}
