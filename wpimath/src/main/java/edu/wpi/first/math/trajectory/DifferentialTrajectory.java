package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.numbers.N6;
import edu.wpi.first.math.system.NumericalIntegration;
import java.util.Arrays;
import java.util.stream.Stream;

/** A trajectory for differential drive robots with drivetrain-specific interpolation. */
public class DifferentialTrajectory extends Trajectory<DifferentialSample> {

  /**
   * Constructs a DifferentialTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  @JsonCreator
  public DifferentialTrajectory(
      @JsonProperty("samples") DifferentialSample[] samples) {
    super(samples);
  }

  public DifferentialTrajectory(
      DifferentialDriveKinematics kinematics, TrajectorySample<?>[] samples) {
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
    double interpTime = MathUtil.lerp(start.timestamp.in(Seconds), end.timestamp.in(Seconds), t);

    double interpDt = interpTime - start.timestamp.in(Seconds);

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
        Seconds.of(interpTime),
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
    return new DifferentialTrajectory(
        Arrays.stream(samples).map(s -> s.transform(transform)).toArray(DifferentialSample[]::new));
  }

  @Override
  public DifferentialTrajectory concatenate(Trajectory<DifferentialSample> other) {
    if (other.samples.length < 1) {
      return this;
    }

    var withNewTimestamp =
        Arrays.stream(other.samples)
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
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
}
