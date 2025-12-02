// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.util.Arrays;
import java.util.stream.Stream;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.kinematics.SwerveDriveKinematics;

/** A trajectory for swerve drive robots with drivetrain-specific interpolation. */
public class SwerveTrajectory extends Trajectory<SwerveSample> {
  private static final ObjectReader reader = new ObjectMapper().readerFor(SwerveTrajectory.class);

  private final SwerveDriveKinematics kinematics;

  /**
   * Constructs a SwerveTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   * @param kinematics the kinematics of the drivetrain.
   */
  @JsonCreator
  public SwerveTrajectory(
      @JsonProperty("kinematics") SwerveDriveKinematics kinematics,
      @JsonProperty("samples") SwerveSample[] samples) {
    super(samples);
    this.kinematics = kinematics;
  }

  /**
   * Constructs a SwerveTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   * @param kinematics the kinematics of the drivetrain.
   */
  public SwerveTrajectory(SwerveDriveKinematics kinematics, TrajectorySample<?>[] samples) {
    this(
        kinematics,
        Arrays.stream(samples)
            .map(s -> new SwerveSample(s, kinematics))
            .toArray(SwerveSample[]::new));
  }

  /**
   * Interpolates between two samples using Euler integration.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  @Override
  public SwerveSample interpolate(SwerveSample start, SwerveSample end, double t) {
    if (start.states.length != end.states.length) {
      throw new IllegalArgumentException(
          "Cannot interpolate SwerveSample with different number of states: "
              + start.states.length
              + " vs "
              + end.states.length);
    }

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, t);

    return new SwerveSample(interpolated, kinematics);
  }

  @Override
  public SwerveTrajectory transformBy(Transform2d transform) {
    Pose2d firstPose = start().pose;
    Pose2d transformedFirstPose = firstPose.transformBy(transform);

    SwerveSample transformedFirstSample =
        new SwerveSample(
            start().timestamp,
            transformedFirstPose,
            start().velocity,
            start().acceleration,
            start().states);

    Stream<SwerveSample> transformedSamples =
        Arrays.stream(samples)
            .skip(1)
            .map(
                sample ->
                    new SwerveSample(
                        sample.timestamp,
                        transformedFirstPose.plus(sample.pose.minus(firstPose)),
                        sample.velocity,
                        sample.acceleration,
                        sample.states));

    return new SwerveTrajectory(
        kinematics,
        Stream.concat(Stream.of(transformedFirstSample), transformedSamples)
            .toArray(SwerveSample[]::new));
  }

  @Override
  public SwerveTrajectory concatenate(Trajectory<SwerveSample> other) {
    if (other.samples.length < 1) {
      return this;
    }

    var withNewTimestamp =
        Arrays.stream(other.samples)
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
            .toArray(SwerveSample[]::new);

    var combinedSamples =
        Stream.concat(Arrays.stream(samples), Arrays.stream(withNewTimestamp))
            .toArray(SwerveSample[]::new);

    return new SwerveTrajectory(kinematics, combinedSamples);
  }

  @Override
  public SwerveTrajectory relativeTo(Pose2d other) {
    return new SwerveTrajectory(
        kinematics,
        Arrays.stream(samples).map(s -> s.relativeTo(other)).toArray(SwerveSample[]::new));
  }

  /**
   * Loads a trajectory from a JSON stream.
   *
   * @param stream the stream to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static SwerveTrajectory loadFromStream(InputStream stream) throws IOException {
    return reader.readValue(stream);
  }

  /**
   * Loads a trajectory from a JSON file.
   *
   * @param file the file to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static SwerveTrajectory loadFromFile(File file) throws IOException {
    return loadFromStream(Files.newInputStream(file.toPath()));
  }

  /**
   * Loads a trajectory from a JSON file.
   *
   * @param filename the path to the file to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static SwerveTrajectory loadFromFile(String filename) throws IOException {
    return loadFromFile(new File(filename));
  }
}
