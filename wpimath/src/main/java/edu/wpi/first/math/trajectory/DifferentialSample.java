package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.MetersPerSecond;
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
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.Time;
import edu.wpi.first.util.struct.StructSerializable;

/** Represents a single sample in a differential drive trajectory. */
public class DifferentialSample extends TrajectorySample<DifferentialSample>
    implements StructSerializable {
  public final double leftSpeed; // meters per second
  public final double rightSpeed; // meters per second\

  /**
   * Constructs a DifferentialSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start, in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   * @param leftSpeed The left-wheel speed at this sample in meters per second.
   * @param rightSpeed The right-wheel speed at this sample in meters per second.
   */
  @JsonCreator
  public DifferentialSample(
      @JsonProperty("timestamp") double timestamp,
      @JsonProperty("pose") Pose2d pose,
      @JsonProperty("velocity") ChassisSpeeds velocity,
      @JsonProperty("acceleration") ChassisAccelerations acceleration,
      @JsonProperty("leftSpeed") double leftSpeed,
      @JsonProperty("rightSpeed") double rightSpeed) {
    super(timestamp, pose, velocity, acceleration);

    this.leftSpeed = leftSpeed;
    this.rightSpeed = rightSpeed;
  }

  /**
   * Constructs a DifferentialSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   * @param leftSpeed The left-wheel speed at this sample in meters per second.
   * @param rightSpeed The right-wheel speed at this sample in meters per second.
   */
  public DifferentialSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds velocity,
      ChassisAccelerations acceleration,
      double leftSpeed,
      double rightSpeed) {
    super(timestamp, pose, velocity, acceleration);

    this.leftSpeed = leftSpeed;
    this.rightSpeed = rightSpeed;
  }

  /**
   * Constructs a DifferentialSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   * @param leftSpeed The left-wheel speed at this sample.
   * @param rightSpeed The right-wheel speed at this sample.
   */
  public DifferentialSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds velocity,
      ChassisAccelerations acceleration,
      LinearVelocity leftSpeed,
      LinearVelocity rightSpeed) {
    this(
        timestamp,
        pose,
        velocity,
        acceleration,
        leftSpeed.in(MetersPerSecond),
        rightSpeed.in(MetersPerSecond));
  }

  /**
   * Constructs a DifferentialSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   * @param kinematics The kinematics of the drivetrain.
   */
  public DifferentialSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds velocity,
      ChassisAccelerations acceleration,
      DifferentialDriveKinematics kinematics) {
    super(timestamp, pose, velocity, acceleration);

    var wheelSpeeds = kinematics.toWheelSpeeds(velocity);
    this.leftSpeed = wheelSpeeds.left;
    this.rightSpeed = wheelSpeeds.right;
  }

  /**
   * Constructs a DifferentialSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param leftSpeed The left-wheel speed at this sample in meters per second.
   * @param rightSpeed The right-wheel speed at this sample in meters per second.
   */
  public DifferentialSample(TrajectorySample<?> sample, double leftSpeed, double rightSpeed) {
    this(
        sample.timestamp, sample.pose, sample.velocity, sample.acceleration, leftSpeed, rightSpeed);
  }

  /**
   * Constructs a DifferentialSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param kinematics The kinematics of the drivetrain.
   */
  public DifferentialSample(TrajectorySample<?> sample, DifferentialDriveKinematics kinematics) {
    this(sample.timestamp, sample.pose, sample.velocity, sample.acceleration, kinematics);
  }

  /**
   * Computes the differential drive dynamics: ẋ = Ax + Bu.
   *
   * @param state [x, y, θ, vₗ, vᵣ, ω]
   * @param input [aₗ, aᵣ, α]
   * @return the state derivatives [vₗ, vᵣ, ω, aₗ, aᵣ, α]
   */
  private Vector<N6> dynamics(Vector<N6> state, Vector<N3> input) {
    double theta = state.get(2);
    double vl = state.get(3);
    double vr = state.get(4);
    double omega = state.get(5);
    double leftAccel = input.get(0);
    double rightAccel = input.get(1);
    double alpha = input.get(2);

    double v = (vl + vr) / 2.0;

    return VecBuilder.fill(
        v * Math.cos(theta), v * Math.sin(theta), omega, leftAccel, rightAccel, alpha);
  }

  /**
   * Matrix version of {@link #dynamics(Vector, Vector)} for use with {@link NumericalIntegration}'s
   * rkdp method.
   */
  private Matrix<N6, N1> dynamics(Matrix<N6, N1> state, Matrix<N3, N1> input) {
    return dynamics(
        new Vector<>(state.extractColumnVector(0)), new Vector<>(input.extractColumnVector(0)));
  }

  /**
   * Interpolates between this sample and the given sample using numerical integration of the
   * differential drive differential equation.
   *
   * @param endValue The end sample.
   * @param t The time between this sample and the end sample. Should be in the range [0, 1].
   * @return new sample
   */
  @Override
  public DifferentialSample interpolate(DifferentialSample endValue, double t) {
    double interpTime =
        MathUtil.lerp(this.timestamp.in(Seconds), endValue.timestamp.in(Seconds), t);

    double interpDt = interpTime - this.timestamp.in(Seconds);

    Matrix<N6, N1> initialState =
        VecBuilder.fill(
            pose.getX(),
            pose.getY(),
            pose.getRotation().getRadians(),
            velocity.vx,
            velocity.vy,
            velocity.omega);

    Vector<N3> initialInput = VecBuilder.fill(acceleration.ax, acceleration.ay, acceleration.alpha);

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

    double ax = MathUtil.lerp(this.acceleration.ax, endValue.acceleration.ax, t);
    double ay = MathUtil.lerp(this.acceleration.ay, endValue.acceleration.ay, t);
    double alpha = MathUtil.lerp(this.acceleration.alpha, endValue.acceleration.alpha, t);

    return new DifferentialSample(
        Seconds.of(interpTime),
        new Pose2d(x, y, Rotation2d.fromRadians(theta)),
        new ChassisSpeeds(vx, vy, omega),
        new ChassisAccelerations(ax, ay, alpha),
        vl,
        vr);
  }

  @Override
  public DifferentialSample transform(Transform2d transform) {
    return new DifferentialSample(
        timestamp, pose.transformBy(transform), velocity, acceleration, leftSpeed, rightSpeed);
  }

  @Override
  public DifferentialSample relativeTo(Pose2d other) {
    return new DifferentialSample(
        timestamp, pose.relativeTo(other), velocity, acceleration, leftSpeed, rightSpeed);
  }

  @Override
  public DifferentialSample withNewTimestamp(Time timestamp) {
    return new DifferentialSample(timestamp, pose, velocity, acceleration, leftSpeed, rightSpeed);
  }
}
