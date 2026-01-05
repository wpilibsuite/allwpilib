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
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.linalg.Vector;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N3;
import org.wpilib.math.numbers.N6;
import org.wpilib.math.system.NumericalIntegration;
import org.wpilib.math.trajectory.proto.DifferentialTrajectoryProto;
import org.wpilib.math.util.MathUtil;

/** A trajectory for differential drive robots with drivetrain-specific interpolation. */
public class DifferentialTrajectory extends Trajectory<DifferentialSample> {
  private static final ObjectReader reader =
      new ObjectMapper().readerFor(DifferentialTrajectory.class);

  /** Base proto for serialization. */
  public static final DifferentialTrajectoryProto proto = new DifferentialTrajectoryProto();

  /**
   * Constructs a DifferentialTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  @JsonCreator
  public DifferentialTrajectory(@JsonProperty("samples") DifferentialSample[] samples) {
    super(samples);
  }

  /**
   * Constructs a DifferentialTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   * @param kinematics the kinematics of the drivetrain.
   */
  public DifferentialTrajectory(
      DifferentialDriveKinematics kinematics, TrajectorySample[] samples) {
    this(
        Arrays.stream(samples)
            .map(s -> new DifferentialSample(s, kinematics))
            .toArray(DifferentialSample[]::new));
  }

  /**
   * Interpolates between two samples using numerical integration of the differential drive
   * differential equation.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  @Override
  public DifferentialSample interpolate(
      DifferentialSample start, DifferentialSample end, double t) {
    double interpTime = MathUtil.lerp(start.timestamp, end.timestamp, t);

    double interpDt = interpTime - start.timestamp;

    Matrix<N6, N1> initialState =
        VecBuilder.fill(
            start.pose.getX(),
            start.pose.getY(),
            start.pose.getRotation().getRadians(),
            start.velocity.vx,
            start.velocity.vy,
            start.velocity.omega);

    Vector<N3> initialInput =
        VecBuilder.fill(start.acceleration.ax, start.acceleration.ay, start.acceleration.alpha);

    // integrate state derivatives [vₗ, vᵣ, ω, aₗ, aᵣ, α] to new states [x, y, θ, vₗ, vᵣ, ω]
    Matrix<N6, N1> endState =
        NumericalIntegration.rkdp(this::dynamics, initialState, initialInput, interpDt);

    double x = endState.get(0, 0);
    double y = endState.get(1, 0);
    double theta = endState.get(2, 0);

    double vl = endState.get(3, 0);
    double vr = endState.get(4, 0);
    double vx = (vl + vr) / 2.0;
    double vy = 0.0;
    double omega = endState.get(5, 0);

    double ax = MathUtil.lerp(start.acceleration.ax, end.acceleration.ax, t);
    double ay = MathUtil.lerp(start.acceleration.ay, end.acceleration.ay, t);
    double alpha = MathUtil.lerp(start.acceleration.alpha, end.acceleration.alpha, t);

    return new DifferentialSample(
        interpTime,
        new Pose2d(x, y, Rotation2d.fromRadians(theta)),
        new ChassisSpeeds(vx, vy, omega),
        new ChassisAccelerations(ax, ay, alpha),
        vl,
        vr);
  }

  /**
   * Computes the differential drive dynamics: ẋ = Ax + Bu.
   *
   * @param state [x, y, θ, vₗ, vᵣ, ω]
   * @param input [aₗ, aᵣ, α]
   * @return the state derivatives [vₗ, vᵣ, ω, aₗ, aᵣ, α]
   */
  private Vector<N6> dynamics(Matrix<N6, N1> state, Matrix<N3, N1> input) {
    double theta = state.get(2, 0);
    double vl = state.get(3, 0);
    double vr = state.get(4, 0);
    double omega = state.get(5, 0);
    double leftAccel = input.get(0, 0);
    double rightAccel = input.get(1, 0);
    double alpha = input.get(2, 0);

    double v = (vl + vr) / 2.0;

    return VecBuilder.fill(
        v * Math.cos(theta), v * Math.sin(theta), omega, leftAccel, rightAccel, alpha);
  }

  @Override
  public DifferentialTrajectory transformBy(Transform2d transform) {
    Pose2d firstPose = start().pose;
    Pose2d transformedFirstPose = firstPose.transformBy(transform);

    DifferentialSample transformedFirstSample =
        new DifferentialSample(
            start().timestamp,
            transformedFirstPose,
            start().velocity,
            start().acceleration,
            start().leftSpeed,
            start().rightSpeed);

    Stream<DifferentialSample> transformedSamples =
        Arrays.stream(samples)
            .skip(1)
            .map(
                sample ->
                    new DifferentialSample(
                        sample.timestamp,
                        transformedFirstPose.plus(sample.pose.minus(firstPose)),
                        sample.velocity,
                        sample.acceleration,
                        sample.leftSpeed,
                        sample.rightSpeed));

    return new DifferentialTrajectory(
        Stream.concat(Stream.of(transformedFirstSample), transformedSamples)
            .toArray(DifferentialSample[]::new));
  }

  @Override
  public DifferentialTrajectory concatenate(Trajectory<DifferentialSample> other) {
    if (other.samples.length < 1) {
      return this;
    }

    var withNewTimestamp =
        Arrays.stream(other.samples)
            .map(s -> s.withNewTimestamp(s.timestamp + this.duration))
            .toArray(DifferentialSample[]::new);

    var combinedSamples =
        Stream.concat(Arrays.stream(samples), Arrays.stream(withNewTimestamp))
            .toArray(DifferentialSample[]::new);

    return new DifferentialTrajectory(combinedSamples);
  }

  @Override
  public DifferentialTrajectory relativeTo(Pose2d other) {
    return new DifferentialTrajectory(
        Arrays.stream(samples).map(s -> s.relativeTo(other)).toArray(DifferentialSample[]::new));
  }

  /**
   * Loads a trajectory from a JSON stream.
   *
   * @param stream the stream to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static DifferentialTrajectory loadFromStream(InputStream stream) throws IOException {
    return reader.readValue(stream);
  }

  /**
   * Loads a trajectory from a JSON file.
   *
   * @param file the file to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static DifferentialTrajectory loadFromFile(File file) throws IOException {
    return loadFromStream(Files.newInputStream(file.toPath()));
  }

  /**
   * Loads a trajectory from a JSON file.
   *
   * @param filename the path to the file to read from
   * @return the deserialized trajectory
   * @throws IOException if there's an error reading the file or parsing the JSON
   */
  public static DifferentialTrajectory loadFromFile(String filename) throws IOException {
    return loadFromFile(new File(filename));
  }
}
