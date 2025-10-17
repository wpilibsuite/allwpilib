package edu.wpi.first.math.trajectory;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectReader;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.util.Arrays;
import java.util.stream.Stream;

/** A trajectory for mecanum drive robots with drivetrain-specific interpolation. */
public class MecanumTrajectory extends Trajectory<MecanumSample> {

  private static final ObjectReader reader = new ObjectMapper().readerFor(MecanumTrajectory.class);

  private final MecanumDriveKinematics kinematics;

  /**
   * Constructs a MecanumTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   * @param kinematics the kinematics of the drivetrain.
   */
  @JsonCreator
  public MecanumTrajectory(
      @JsonProperty("kinematics") MecanumDriveKinematics kinematics,
      @JsonProperty("samples") MecanumSample[] samples) {
    super(samples);
    this.kinematics = kinematics;
  }

  /**
   * Constructs a MecanumTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   * @param kinematics the kinematics of the drivetrain.
   */
  public MecanumTrajectory(MecanumDriveKinematics kinematics, TrajectorySample<?>[] samples) {
    this(
        kinematics,
        Arrays.stream(samples)
            .map(s -> new MecanumSample(s, kinematics))
            .toArray(MecanumSample[]::new));
  }

  /**
   * Linearly interpolates between two samples.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  @Override
  public MecanumSample interpolate(MecanumSample start, MecanumSample end, double t) {
    TrajectorySample.Base interpolated = TrajectorySample.kinematicInterpolate(start, end, t);

    return new MecanumSample(interpolated, kinematics);
  }

  @Override
  public MecanumTrajectory transformBy(Transform2d transform) {
    Pose2d firstPose = start().pose;
    Pose2d transformedFirstPose = firstPose.transformBy(transform);

    MecanumSample transformedFirstSample =
        new MecanumSample(
            start().timestamp,
            transformedFirstPose,
            start().velocity,
            start().acceleration,
            start().speeds);

    Stream<MecanumSample> transformedSamples =
        Arrays.stream(samples)
            .skip(1)
            .map(
                sample ->
                    new MecanumSample(
                        sample.timestamp,
                        transformedFirstPose.plus(sample.pose.minus(firstPose)),
                        sample.velocity,
                        sample.acceleration,
                        sample.speeds));

    return new MecanumTrajectory(
        kinematics,
        Stream.concat(Stream.of(transformedFirstSample), transformedSamples)
            .toArray(MecanumSample[]::new));
  }

  @Override
  public MecanumTrajectory concatenate(Trajectory<MecanumSample> other) {
    if (other.samples.length < 1) {
      return this;
    }

    var withNewTimestamp =
        Arrays.stream(other.samples)
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
            .toArray(MecanumSample[]::new);

    var combinedSamples =
        Stream.concat(Arrays.stream(samples), Arrays.stream(withNewTimestamp))
            .toArray(MecanumSample[]::new);

    return new MecanumTrajectory(kinematics, combinedSamples);
  }

  @Override
  public MecanumTrajectory relativeTo(Pose2d other) {
    return new MecanumTrajectory(
        kinematics,
        Arrays.stream(samples).map(s -> s.relativeTo(other)).toArray(MecanumSample[]::new));
  }

  /**
   * Loads a trajectory from a JSON stream.
   *
   * @param stream the stream to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static MecanumTrajectory loadFromStream(InputStream stream) throws IOException {
    return reader.readValue(stream);
  }

  /**
   * Loads a trajectory from a JSON file.
   *
   * @param file the file to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static MecanumTrajectory loadFromFile(File file) throws IOException {
    return loadFromStream(Files.newInputStream(file.toPath()));
  }

  /**
   * Loads a trajectory from a JSON file.
   *
   * @param filename the path to the file to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static MecanumTrajectory loadFromFile(String filename) throws IOException {
    return loadFromFile(new File(filename));
  }
}
