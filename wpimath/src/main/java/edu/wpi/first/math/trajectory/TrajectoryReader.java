package edu.wpi.first.math.trajectory;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectReader;
import java.io.File;
import java.io.IOException;

/**
 * Utility class for reading trajectory objects from JSON files and strings.
 *
 * <p>This class provides static methods to deserialize different types of trajectories from JSON
 * format. It supports loading trajectories from files, file paths (as strings), and JSON strings
 * directly.
 *
 * <p>Supported trajectory types:
 *
 * <ul>
 *   <li>{@link TrajectoryBase} - Base trajectory with samples that do not contain
 *       drivetrain-specific information
 *   <li>{@link DifferentialTrajectory} - Trajectory for differential drive robots
 *   <li>{@link MecanumTrajectory} - Trajectory for mecanum drive robots
 *   <li>{@link SwerveTrajectory} - Trajectory for swerve drive robots
 * </ul>
 *
 * <p>Example usage:
 *
 * <pre>{@code
 * // Load from file
 * TrajectoryBase trajectory = TrajectoryReader.load(new File("path/to/trajectory.json"));
 *
 * // Load from file path
 * DifferentialTrajectory diffTraj =
 *   TrajectoryReader.loadDifferential("path/to/diff_trajectory.json");
 *
 * // Load from JSON string
 * String jsonString = "{...}";
 * SwerveTrajectory swerveTraj = TrajectoryReader.loadSwerveFromJsonString(jsonString);
 * }</pre>
 */
public final class TrajectoryReader {
  private TrajectoryReader() {
    throw new AssertionError("utility class");
  }

  private static final ObjectReader reader = new ObjectMapper().reader();

  /**
   * Loads a base trajectory from a JSON file.
   *
   * @param file the file containing the JSON trajectory data
   * @return the deserialized {@link TrajectoryBase} object
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static TrajectoryBase load(File file) throws IOException {
    return reader.readValue(file, TrajectoryBase.class);
  }

  /**
   * Loads a base trajectory from a JSON file specified by filename.
   *
   * @param filename the path to the file containing the JSON trajectory data
   * @return the deserialized {@link TrajectoryBase} object
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static TrajectoryBase load(String filename) throws IOException {
    return reader.readValue(new File(filename), TrajectoryBase.class);
  }

  /**
   * Loads a base trajectory from a JSON string.
   *
   * @param json the JSON string containing trajectory data
   * @return the deserialized {@link TrajectoryBase} object
   * @throws IOException if there's an error parsing the JSON
   */
  public static TrajectoryBase loadFromJsonString(String json) throws IOException {
    return reader.readValue(json, TrajectoryBase.class);
  }

  /**
   * Loads a differential drive trajectory from a JSON file.
   *
   * @param file the file containing the JSON trajectory data
   * @return the deserialized {@link DifferentialTrajectory} object
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static DifferentialTrajectory loadDifferential(File file) throws IOException {
    return reader.readValue(file, DifferentialTrajectory.class);
  }

  /**
   * Loads a differential drive trajectory from a JSON file specified by filename.
   *
   * @param filename the path to the file containing the JSON trajectory data
   * @return the deserialized {@link DifferentialTrajectory} object
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static DifferentialTrajectory loadDifferential(String filename) throws IOException {
    return reader.readValue(new File(filename), DifferentialTrajectory.class);
  }

  /**
   * Loads a differential drive trajectory from a JSON string.
   *
   * @param json the JSON string containing trajectory data
   * @return the deserialized {@link DifferentialTrajectory} object
   * @throws IOException if there's an error parsing the JSON
   */
  public static DifferentialTrajectory loadDifferentialFromJsonString(String json)
      throws IOException {
    return reader.readValue(json, DifferentialTrajectory.class);
  }

  /**
   * Loads a mecanum drive trajectory from a JSON file.
   *
   * @param file the file containing the JSON trajectory data
   * @return the deserialized {@link MecanumTrajectory} object
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static MecanumTrajectory loadMecanum(File file) throws IOException {
    return reader.readValue(file, MecanumTrajectory.class);
  }

  /**
   * Loads a mecanum drive trajectory from a JSON file specified by filename.
   *
   * @param filename the path to the file containing the JSON trajectory data
   * @return the deserialized {@link MecanumTrajectory} object
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static MecanumTrajectory loadMecanum(String filename) throws IOException {
    return reader.readValue(new File(filename), MecanumTrajectory.class);
  }

  /**
   * Loads a mecanum drive trajectory from a JSON string.
   *
   * @param json the JSON string containing trajectory data
   * @return the deserialized {@link MecanumTrajectory} object
   * @throws IOException if there's an error parsing the JSON
   */
  public static MecanumTrajectory loadMecanumFromJsonString(String json) throws IOException {
    return reader.readValue(json, MecanumTrajectory.class);
  }

  /**
   * Loads a swerve drive trajectory from a JSON file.
   *
   * @param file the file containing the JSON trajectory data
   * @return the deserialized {@link SwerveTrajectory} object
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static SwerveTrajectory loadSwerve(File file) throws IOException {
    return reader.readValue(file, SwerveTrajectory.class);
  }

  /**
   * Loads a swerve drive trajectory from a JSON file specified by filename.
   *
   * @param filename the path to the file containing the JSON trajectory data
   * @return the deserialized {@link SwerveTrajectory} object
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static SwerveTrajectory loadSwerve(String filename) throws IOException {
    return reader.readValue(new File(filename), SwerveTrajectory.class);
  }

  /**
   * Loads a swerve drive trajectory from a JSON string.
   *
   * @param json the JSON string containing trajectory data
   * @return the deserialized {@link SwerveTrajectory} object
   * @throws IOException if there's an error parsing the JSON
   */
  public static SwerveTrajectory loadSwerveFromJsonString(String json) throws IOException {
    return reader.readValue(json, SwerveTrajectory.class);
  }
}
