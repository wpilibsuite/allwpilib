// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static edu.wpi.first.units.Units.Meters;

import org.wpilib.math.util.MathSharedStore;
import org.wpilib.math.geometry.Twist2d;
import org.wpilib.math.kinematics.proto.DifferentialDriveKinematicsProto;
import org.wpilib.math.kinematics.struct.DifferentialDriveKinematicsStruct;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/**
 * Helper class that converts a chassis velocity (dx and dtheta components) to left and right wheel
 * velocities for a differential drive.
 *
 * <p>Inverse kinematics converts a desired chassis speed into left and right velocity components
 * whereas forward kinematics converts left and right component velocities into a linear and angular
 * chassis speed.
 */
public class DifferentialDriveKinematics
    implements Kinematics<DifferentialDriveWheelSpeeds, DifferentialDriveWheelPositions>,
        ProtobufSerializable,
        StructSerializable {
  /** Differential drive trackwidth in meters. */
  public final double trackwidth;

  /** DifferentialDriveKinematics protobuf for serialization. */
  public static final DifferentialDriveKinematicsProto proto =
      new DifferentialDriveKinematicsProto();

  /** DifferentialDriveKinematics struct for serialization. */
  public static final DifferentialDriveKinematicsStruct struct =
      new DifferentialDriveKinematicsStruct();

  /**
   * Constructs a differential drive kinematics object.
   *
   * @param trackwidth The trackwidth of the drivetrain in meters. Theoretically, this is the
   *     distance between the left wheels and right wheels. However, the empirical value may be
   *     larger than the physical measured value due to scrubbing effects.
   */
  public DifferentialDriveKinematics(double trackwidth) {
    this.trackwidth = trackwidth;
    MathSharedStore.reportUsage("DifferentialDriveKinematics", "");
  }

  /**
   * Constructs a differential drive kinematics object.
   *
   * @param trackwidth The trackwidth of the drivetrain in meters. Theoretically, this is the
   *     distance between the left wheels and right wheels. However, the empirical value may be
   *     larger than the physical measured value due to scrubbing effects.
   */
  public DifferentialDriveKinematics(Distance trackwidth) {
    this(trackwidth.in(Meters));
  }

  /**
   * Returns a chassis speed from left and right component velocities using forward kinematics.
   *
   * @param wheelSpeeds The left and right velocities.
   * @return The chassis speed.
   */
  @Override
  public ChassisSpeeds toChassisSpeeds(DifferentialDriveWheelSpeeds wheelSpeeds) {
    return new ChassisSpeeds(
        (wheelSpeeds.left + wheelSpeeds.right) / 2,
        0,
        (wheelSpeeds.right - wheelSpeeds.left) / trackwidth);
  }

  /**
   * Returns left and right component velocities from a chassis speed using inverse kinematics.
   *
   * @param chassisSpeeds The linear and angular (dx and dtheta) components that represent the
   *     chassis' speed.
   * @return The left and right velocities.
   */
  @Override
  public DifferentialDriveWheelSpeeds toWheelSpeeds(ChassisSpeeds chassisSpeeds) {
    return new DifferentialDriveWheelSpeeds(
        chassisSpeeds.vx - trackwidth / 2 * chassisSpeeds.omega,
        chassisSpeeds.vx + trackwidth / 2 * chassisSpeeds.omega);
  }

  @Override
  public Twist2d toTwist2d(
      DifferentialDriveWheelPositions start, DifferentialDriveWheelPositions end) {
    return toTwist2d(end.left - start.left, end.right - start.right);
  }

  /**
   * Performs forward kinematics to return the resulting Twist2d from the given left and right side
   * distance deltas. This method is often used for odometry -- determining the robot's position on
   * the field using changes in the distance driven by each wheel on the robot.
   *
   * @param leftDistance The distance measured by the left side encoder in meters.
   * @param rightDistance The distance measured by the right side encoder in meters.
   * @return The resulting Twist2d.
   */
  public Twist2d toTwist2d(double leftDistance, double rightDistance) {
    return new Twist2d(
        (leftDistance + rightDistance) / 2, 0, (rightDistance - leftDistance) / trackwidth);
  }

  @Override
  public DifferentialDriveWheelPositions copy(DifferentialDriveWheelPositions positions) {
    return new DifferentialDriveWheelPositions(positions.left, positions.right);
  }

  @Override
  public void copyInto(
      DifferentialDriveWheelPositions positions, DifferentialDriveWheelPositions output) {
    output.left = positions.left;
    output.right = positions.right;
  }

  @Override
  public DifferentialDriveWheelPositions interpolate(
      DifferentialDriveWheelPositions startValue,
      DifferentialDriveWheelPositions endValue,
      double t) {
    return startValue.interpolate(endValue, t);
  }
}
