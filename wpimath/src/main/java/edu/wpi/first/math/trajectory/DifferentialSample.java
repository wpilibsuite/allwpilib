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
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N6;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.Time;
import edu.wpi.first.util.struct.StructSerializable;

/** Represents a single sample in a differential drive trajectory. */
public class DifferentialSample extends TrajectorySample<DifferentialSample>
    implements StructSerializable {
  public final double leftSpeed; // meters per second
  public final double rightSpeed; // meters per second

  /**
   * Constructs a DifferentialSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start, in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param vel The robot velocity at this sample (in the robot's reference frame).
   * @param accel The robot acceleration at this sample (in the robot's reference frame).
   * @param leftSpeed The left-wheel speed at this sample in meters per second.
   * @param rightSpeed The right-wheel speed at this sample in meters per second.
   */
  @JsonCreator
  public DifferentialSample(
      @JsonProperty("timestamp") double timestamp,
      @JsonProperty("pose") Pose2d pose,
      @JsonProperty("vel") ChassisSpeeds vel,
      @JsonProperty("accel") ChassisAccelerations accel,
      @JsonProperty("leftSpeed") double leftSpeed,
      @JsonProperty("rightSpeed") double rightSpeed) {
    super(timestamp, pose, vel, accel);

    this.leftSpeed = leftSpeed;
    this.rightSpeed = rightSpeed;
  }

  /**
   * Constructs a DifferentialSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param vel The robot velocity at this sample (in the robot's reference frame).
   * @param accel The robot acceleration at this sample (in the robot's reference frame).
   * @param leftSpeed The left-wheel speed at this sample in meters per second.
   * @param rightSpeed The right-wheel speed at this sample in meters per second.
   */
  public DifferentialSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds vel,
      ChassisAccelerations accel,
      double leftSpeed,
      double rightSpeed) {
    super(timestamp, pose, vel, accel);

    this.leftSpeed = leftSpeed;
    this.rightSpeed = rightSpeed;
  }

  /**
   * Constructs a DifferentialSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param vel The robot velocity at this sample (in the robot's reference frame).
   * @param accel The robot acceleration at this sample (in the robot's reference frame).
   * @param leftSpeed The left-wheel speed at this sample.
   * @param rightSpeed The right-wheel speed at this sample.
   */
  public DifferentialSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds vel,
      ChassisAccelerations accel,
      LinearVelocity leftSpeed,
      LinearVelocity rightSpeed) {
    this(
        timestamp, pose, vel, accel, leftSpeed.in(MetersPerSecond), rightSpeed.in(MetersPerSecond));
  }

  /**
   * Constructs a DifferentialSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param vel The robot velocity at this sample (in the robot's reference frame).
   * @param accel The robot acceleration at this sample (in the robot's reference frame).
   * @param kinematics The kinematics of the drivetrain.
   */
  public DifferentialSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds vel,
      ChassisAccelerations accel,
      DifferentialDriveKinematics kinematics) {
    super(timestamp, pose, vel, accel);

    var wheelSpeeds = kinematics.toWheelSpeeds(vel);
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
    this(sample.timestamp, sample.pose, sample.vel, sample.accel, leftSpeed, rightSpeed);
  }

  /**
   * Constructs a DifferentialSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param kinematics The kinematics of the drivetrain.
   */
  public DifferentialSample(TrajectorySample<?> sample, DifferentialDriveKinematics kinematics) {
    this(sample.timestamp, sample.pose, sample.vel, sample.accel, kinematics);
  }

  /**
   * Computes the state derivatives from the input vectors.
   *
   * @param state [x, y, θ, vₗ, vᵣ, ω]
   * @param input [aₗ, aᵣ]
   * @return the state derivatives [vₗ, vᵣ, ω, aₗ, aᵣ, α]
   */
  private Vector<N6> computeStateDerivatives(Vector<N6> state, Vector<N2> input) {
    double theta = state.get(2);
    double vl = state.get(3);
    double vr = state.get(4);
    double omega = state.get(5);
    double leftAccel = input.get(0);
    double rightAccel = input.get(1);

    double vx = (vl + vr) / 2.0;
    double trackwidth = (vr - vl) / omega;
    double alpha = (rightAccel - leftAccel) / trackwidth;

    return VecBuilder.fill(
        vx * Math.cos(theta), vx * Math.sin(theta), omega, leftAccel, rightAccel, alpha);
  }

  /**
   * Matrix version of {@link #computeStateDerivatives(Vector, Vector)} for use with {@link
   * NumericalIntegration}'s rkdp method.
   */
  private Matrix<N6, N1> computeStateDerivatives(Matrix<N6, N1> state, Matrix<N2, N1> input) {
    return computeStateDerivatives(
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
        MathUtil.interpolate(this.timestamp.in(Seconds), endValue.timestamp.in(Seconds), t);

    double interpDt = interpTime - this.timestamp.in(Seconds);

    Matrix<N6, N1> initialState =
        VecBuilder.fill(
            pose.getX(), pose.getY(), pose.getRotation().getRadians(), vel.vx, vel.vy, vel.omega);

    Matrix<N2, N1> initialInput = VecBuilder.fill(accel.ax, accel.ay);

    // integrate state derivatives [vₗ, vᵣ, ω, aₗ, aᵣ, α] to new states [x, y, θ, vₗ, vᵣ, ω]
    Matrix<N6, N1> endState =
        NumericalIntegration.rkdp(
            this::computeStateDerivatives, initialState, initialInput, interpDt);

    double x = endState.get(0, 0);
    double y = endState.get(1, 0);
    double theta = endState.get(2, 0);

    double vl = endState.get(3, 0);
    double vr = endState.get(4, 0);
    double vx = (vl + vr) / 2.0;
    double vy = 0.0;
    double omega = endState.get(5, 0);

    double ax = MathUtil.interpolate(this.accel.ax, endValue.accel.ax, t);
    double ay = MathUtil.interpolate(this.accel.ay, endValue.accel.ay, t);
    double alpha = MathUtil.interpolate(this.accel.alpha, endValue.accel.alpha, t);

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
        timestamp, pose.transformBy(transform), vel, accel, leftSpeed, rightSpeed
    );
  }
}
