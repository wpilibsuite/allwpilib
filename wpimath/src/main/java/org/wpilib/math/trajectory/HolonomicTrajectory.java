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
import java.util.List;
import java.util.stream.Stream;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Transform2d;

/** A base trajectory class for general-purpose trajectory following. */
public class HolonomicTrajectory extends Trajectory<TrajectorySample> {
  private static final ObjectReader reader = new ObjectMapper().readerFor(HolonomicTrajectory.class);

  /**
   * Constructs a BaseTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  @JsonCreator
  public HolonomicTrajectory(@JsonProperty("samples") TrajectorySample[] samples) {
    super(samples);
  }

  public HolonomicTrajectory(List<? extends TrajectorySample> samples) {
    this(samples.toArray(TrajectorySample[]::new));
  }

  /**
   * Interpolates between two samples using constant-acceleration kinematic equations.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  @Override
  public TrajectorySample interpolate(TrajectorySample start, TrajectorySample end, double t) {
    return TrajectorySample.kinematicInterpolate(start, end, t);
  }

  @Override
  public HolonomicTrajectory transformBy(Transform2d transform) {
    Pose2d firstPose = start().pose;
    Pose2d transformedFirstPose = firstPose.transformBy(transform);

    TrajectorySample transformedFirstSample =
        new TrajectorySample(
            start().timestamp, transformedFirstPose, start().velocity, start().acceleration);

    Stream<TrajectorySample> transformedSamples =
        Arrays.stream(samples)
            .skip(1)
            .map(
                sample ->
                    new TrajectorySample(
                        sample.timestamp,
                        transformedFirstPose.plus(sample.pose.minus(firstPose)),
                        sample.velocity,
                        sample.acceleration));

    return new HolonomicTrajectory(
        Stream.concat(Stream.of(transformedFirstSample), transformedSamples)
            .toArray(TrajectorySample[]::new));
  }

  @Override
  public HolonomicTrajectory concatenate(Trajectory<TrajectorySample> other) {
    if (other.samples.length < 1) {
      return this;
    }

    var withNewTimestamp =
        Arrays.stream(other.samples)
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
            .toArray(TrajectorySample[]::new);

    var combinedSamples =
        Stream.concat(Arrays.stream(samples), Arrays.stream(withNewTimestamp))
            .toArray(TrajectorySample[]::new);

    return new HolonomicTrajectory(combinedSamples);
  }

  @Override
  public HolonomicTrajectory relativeTo(Pose2d other) {
    return new HolonomicTrajectory(
        Arrays.stream(samples).map(s -> s.relativeTo(other)).toArray(TrajectorySample[]::new));
  }

  /**
   * Loads a trajectory from a JSON stream.
   *
   * @param stream the stream to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static HolonomicTrajectory loadFromStream(InputStream stream) throws IOException {
    return reader.readValue(stream);
  }

  /**
   * Loads a trajectory from a JSON file.
   *
   * @param file the file to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static HolonomicTrajectory loadFromFile(File file) throws IOException {
    return loadFromStream(Files.newInputStream(file.toPath()));
  }

  /**
   * Loads a trajectory from a JSON file.
   *
   * @param filename the path to the file to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static HolonomicTrajectory loadFromFile(String filename) throws IOException {
    return loadFromFile(new File(filename));
  }
}
