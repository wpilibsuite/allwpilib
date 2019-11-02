/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectReader;
import com.fasterxml.jackson.databind.ObjectWriter;

public final class TrajectoryUtil {
  private static final ObjectReader READER = new ObjectMapper().readerFor(Trajectory.State[].class);
  private static final ObjectWriter WRITER = new ObjectMapper().writerFor(Trajectory.State[].class);

  private TrajectoryUtil() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Imports a Trajectory from a PathWeaver-style JSON file.
   * @param path the path of the json file to import from
   * @return The trajectory represented by the file.
   * @throws IOException if reading from the file fails
   */
  public static Trajectory fromPathweaverJson(Path path) throws IOException {
    try (BufferedReader reader = Files.newBufferedReader(path)) {
      Trajectory.State[] state = READER.readValue(reader);
      return new Trajectory(Arrays.asList(state));
    }
  }

  /**
   * Exports a Trajectory to a PathWeaver-style JSON file.
   * @param trajectory the trajectory to export
   * @param path the path of the file to export to
   * @throws IOException if writing to the file fails
   */
  public static void toPathweaverJson(Trajectory trajectory, Path path) throws IOException {
    try (BufferedWriter writer = Files.newBufferedWriter(path)) {
      WRITER.writeValue(writer, trajectory.getStates().toArray(new Trajectory.State[0]));
    }
  }

  /**
   * Deserializes a Trajectory from PathWeaver-style JSON.
   * @param json the string containing the serialized JSON
   * @return the trajectory represented by the JSON
   * @throws JsonProcessingException if deserializing the JSON fails
   */
  public static Trajectory deserializeTrajectory(String json) throws JsonProcessingException {
    Trajectory.State[] state = READER.readValue(json);
    return new Trajectory(Arrays.asList(state));
  }

  /**
   * Serializes a Trajectory to PathWeaver-style JSON.
   * @param trajectory the trajectory to export
   * @return the string containing the serialized JSON
   * @throws JsonProcessingException if serializing the Trajectory fails
   */
  public static String serializeTrajectory(Trajectory trajectory) throws JsonProcessingException {
    return WRITER.writeValueAsString(trajectory.getStates().toArray(new Trajectory.State[0]));
  }
}
