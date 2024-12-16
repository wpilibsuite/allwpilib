// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecond;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.kinematics.proto.ChassisSpeedsProto;
import edu.wpi.first.math.kinematics.struct.ChassisSpeedsStruct;
import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/**
 * Represents the speed of a robot chassis. Although this class contains similar members compared to
 * a Twist2d, they do NOT represent the same thing. Whereas a Twist2d represents a change in pose
 * w.r.t to the robot frame of reference, a ChassisSpeeds object represents a robot's velocity.
 *
 * <p>A strictly non-holonomic drivetrain, such as a differential drive, should never have a dy
 * component because it can never move sideways. Holonomic drivetrains such as swerve and mecanum
 * will often have all three components.
 */
public class ChassisSpeeds implements ProtobufSerializable, StructSerializable {
  /** Velocity along the x-axis. (Fwd is +) */
  public double vxMetersPerSecond;

  /** Velocity along the y-axis. (Left is +) */
  public double vyMetersPerSecond;

  /** Represents the angular velocity of the robot frame. (CCW is +) */
  public double omegaRadiansPerSecond;

  /** ChassisSpeeds protobuf for serialization. */
  public static final ChassisSpeedsProto proto = new ChassisSpeedsProto();

  /** ChassisSpeeds struct for serialization. */
  public static final ChassisSpeedsStruct struct = new ChassisSpeedsStruct();

  /** Constructs a ChassisSpeeds with zeros for dx, dy, and theta. */
  public ChassisSpeeds() {}

  /**
   * Constructs a ChassisSpeeds object.
   *
   * @param vxMetersPerSecond Forward velocity.
   * @param vyMetersPerSecond Sideways velocity.
   * @param omegaRadiansPerSecond Angular velocity.
   */
  public ChassisSpeeds(
      double vxMetersPerSecond, double vyMetersPerSecond, double omegaRadiansPerSecond) {
    this.vxMetersPerSecond = vxMetersPerSecond;
    this.vyMetersPerSecond = vyMetersPerSecond;
    this.omegaRadiansPerSecond = omegaRadiansPerSecond;
  }

  /**
   * Constructs a ChassisSpeeds object.
   *
   * @param vx Forward velocity.
   * @param vy Sideways velocity.
   * @param omega Angular velocity.
   */
  public ChassisSpeeds(LinearVelocity vx, LinearVelocity vy, AngularVelocity omega) {
    this(vx.in(MetersPerSecond), vy.in(MetersPerSecond), omega.in(RadiansPerSecond));
  }

  /**
   * Creates a Twist2d from ChassisSpeeds.
   *
   * @param dtSeconds The duration of the timestep.
   * @return Twist2d.
   */
  public Twist2d toTwist2d(double dtSeconds) {
    return new Twist2d(
        vxMetersPerSecond * dtSeconds,
        vyMetersPerSecond * dtSeconds,
        omegaRadiansPerSecond * dtSeconds);
  }

  /**
   * Discretizes a continuous-time chassis speed.
   *
   * <p>This function converts this continuous-time chassis speed into a discrete-time one such that
   * when the discrete-time chassis speed is applied for one timestep, the robot moves as if the
   * velocity components are independent (i.e., the robot moves v_x * dt along the x-axis, v_y * dt
   * along the y-axis, and omega * dt around the z-axis).
   *
   * <p>This is useful for compensating for translational skew when translating and rotating a
   * holonomic (swerve or mecanum) drivetrain. However, scaling down the ChassisSpeeds after
   * discretizing (e.g., when desaturating swerve module speeds) rotates the direction of net motion
   * in the opposite direction of rotational velocity, introducing a different translational skew
   * which is not accounted for by discretization.
   *
   * @param dtSeconds The duration of the timestep the speeds should be applied for.
   * @return Discretized ChassisSpeeds.
   */
  public ChassisSpeeds discretize(double dtSeconds) {
    var desiredDeltaPose =
        new Pose2d(
            vxMetersPerSecond * dtSeconds,
            vyMetersPerSecond * dtSeconds,
            new Rotation2d(omegaRadiansPerSecond * dtSeconds));

    // Find the chassis translation/rotation deltas in the robot frame that move the robot from its
    // current pose to the desired pose
    var twist = Pose2d.kZero.log(desiredDeltaPose);

    // Turn the chassis translation/rotation deltas into average velocities
    return new ChassisSpeeds(twist.dx / dtSeconds, twist.dy / dtSeconds, twist.dtheta / dtSeconds);
  }

  /**
   * Converts this field-relative set of speeds into a robot-relative ChassisSpeeds object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The robot's angle is
   *     considered to be zero when it is facing directly away from your alliance station wall.
   *     Remember that this should be CCW positive.
   * @return ChassisSpeeds object representing the speeds in the robot's frame of reference.
   */
  public ChassisSpeeds toRobotRelative(Rotation2d robotAngle) {
    // CW rotation into chassis frame
    var rotated =
        new Translation2d(vxMetersPerSecond, vyMetersPerSecond).rotateBy(robotAngle.unaryMinus());
    return new ChassisSpeeds(rotated.getX(), rotated.getY(), omegaRadiansPerSecond);
  }

  /**
   * Converts this robot-relative set of speeds into a field-relative ChassisSpeeds object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The robot's angle is
   *     considered to be zero when it is facing directly away from your alliance station wall.
   *     Remember that this should be CCW positive.
   * @return ChassisSpeeds object representing the speeds in the field's frame of reference.
   */
  public ChassisSpeeds toFieldRelative(Rotation2d robotAngle) {
    // CCW rotation out of chassis frame
    var rotated = new Translation2d(vxMetersPerSecond, vyMetersPerSecond).rotateBy(robotAngle);
    return new ChassisSpeeds(rotated.getX(), rotated.getY(), omegaRadiansPerSecond);
  }

  /**
   * Adds two ChassisSpeeds and returns the sum.
   *
   * <p>For example, ChassisSpeeds{1.0, 0.5, 0.75} + ChassisSpeeds{2.0, 1.5, 0.25} =
   * ChassisSpeeds{3.0, 2.0, 1.0}
   *
   * @param other The ChassisSpeeds to add.
   * @return The sum of the ChassisSpeeds.
   */
  public ChassisSpeeds plus(ChassisSpeeds other) {
    return new ChassisSpeeds(
        vxMetersPerSecond + other.vxMetersPerSecond,
        vyMetersPerSecond + other.vyMetersPerSecond,
        omegaRadiansPerSecond + other.omegaRadiansPerSecond);
  }

  /**
   * Subtracts the other ChassisSpeeds from the current ChassisSpeeds and returns the difference.
   *
   * <p>For example, ChassisSpeeds{5.0, 4.0, 2.0} - ChassisSpeeds{1.0, 2.0, 1.0} =
   * ChassisSpeeds{4.0, 2.0, 1.0}
   *
   * @param other The ChassisSpeeds to subtract.
   * @return The difference between the two ChassisSpeeds.
   */
  public ChassisSpeeds minus(ChassisSpeeds other) {
    return new ChassisSpeeds(
        vxMetersPerSecond - other.vxMetersPerSecond,
        vyMetersPerSecond - other.vyMetersPerSecond,
        omegaRadiansPerSecond - other.omegaRadiansPerSecond);
  }

  /**
   * Returns the inverse of the current ChassisSpeeds. This is equivalent to negating all components
   * of the ChassisSpeeds.
   *
   * @return The inverse of the current ChassisSpeeds.
   */
  public ChassisSpeeds unaryMinus() {
    return new ChassisSpeeds(-vxMetersPerSecond, -vyMetersPerSecond, -omegaRadiansPerSecond);
  }

  /**
   * Multiplies the ChassisSpeeds by a scalar and returns the new ChassisSpeeds.
   *
   * <p>For example, ChassisSpeeds{2.0, 2.5, 1.0} * 2 = ChassisSpeeds{4.0, 5.0, 1.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled ChassisSpeeds.
   */
  public ChassisSpeeds times(double scalar) {
    return new ChassisSpeeds(
        vxMetersPerSecond * scalar, vyMetersPerSecond * scalar, omegaRadiansPerSecond * scalar);
  }

  /**
   * Divides the ChassisSpeeds by a scalar and returns the new ChassisSpeeds.
   *
   * <p>For example, ChassisSpeeds{2.0, 2.5, 1.0} / 2 = ChassisSpeeds{1.0, 1.25, 0.5}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled ChassisSpeeds.
   */
  public ChassisSpeeds div(double scalar) {
    return new ChassisSpeeds(
        vxMetersPerSecond / scalar, vyMetersPerSecond / scalar, omegaRadiansPerSecond / scalar);
  }

  @Override
  public final int hashCode() {
    return Objects.hash(vxMetersPerSecond, vyMetersPerSecond, omegaRadiansPerSecond);
  }

  @Override
  public boolean equals(Object o) {
    return o == this
        || o instanceof ChassisSpeeds c
            && vxMetersPerSecond == c.vxMetersPerSecond
            && vyMetersPerSecond == c.vyMetersPerSecond
            && omegaRadiansPerSecond == c.omegaRadiansPerSecond;
  }

  @Override
  public String toString() {
    return String.format(
        "ChassisSpeeds(Vx: %.2f m/s, Vy: %.2f m/s, Omega: %.2f rad/s)",
        vxMetersPerSecond, vyMetersPerSecond, omegaRadiansPerSecond);
  }
}
