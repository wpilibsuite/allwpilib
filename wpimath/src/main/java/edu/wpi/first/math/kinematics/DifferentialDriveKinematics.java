// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.Meters;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.kinematics.proto.DifferentialDriveKinematicsProto;
import edu.wpi.first.math.kinematics.struct.DifferentialDriveKinematicsStruct;
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
  /** Differential drive trackwidth. */
  public final double trackWidthMeters;

  /** DifferentialDriveKinematics protobuf for serialization. */
  public static final DifferentialDriveKinematicsProto proto =
      new DifferentialDriveKinematicsProto();

  /** DifferentialDriveKinematics struct for serialization. */
  public static final DifferentialDriveKinematicsStruct struct =
      new DifferentialDriveKinematicsStruct();

  /**
   * Constructs a differential drive kinematics object.
   *
   * @param trackWidthMeters The track width of the drivetrain. Theoretically, this is the distance
   *     between the left wheels and right wheels. However, the empirical value may be larger than
   *     the physical measured value due to scrubbing effects.
   */
  public DifferentialDriveKinematics(double trackWidthMeters) {
    this.trackWidthMeters = trackWidthMeters;
    MathSharedStore.reportUsage(MathUsageId.kKinematics_DifferentialDrive, 1);
  }

  /**
   * Constructs a differential drive kinematics object.
   *
   * @param trackWidth The track width of the drivetrain. Theoretically, this is the distance
   *     between the left wheels and right wheels. However, the empirical value may be larger than
   *     the physical measured value due to scrubbing effects.
   */
  public DifferentialDriveKinematics(Distance trackWidth) {
    this(trackWidth.in(Meters));
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
        (wheelSpeeds.leftMetersPerSecond + wheelSpeeds.rightMetersPerSecond) / 2,
        0,
        (wheelSpeeds.rightMetersPerSecond - wheelSpeeds.leftMetersPerSecond) / trackWidthMeters);
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
        chassisSpeeds.vxMetersPerSecond
            - trackWidthMeters / 2 * chassisSpeeds.omegaRadiansPerSecond,
        chassisSpeeds.vxMetersPerSecond
            + trackWidthMeters / 2 * chassisSpeeds.omegaRadiansPerSecond);
  }

  @Override
  public Twist2d toTwist2d(
      DifferentialDriveWheelPositions start, DifferentialDriveWheelPositions end) {
    return toTwist2d(end.leftMeters - start.leftMeters, end.rightMeters - start.rightMeters);
  }

  /**
   * Performs forward kinematics to return the resulting Twist2d from the given left and right side
   * distance deltas. This method is often used for odometry -- determining the robot's position on
   * the field using changes in the distance driven by each wheel on the robot.
   *
   * @param leftDistanceMeters The distance measured by the left side encoder.
   * @param rightDistanceMeters The distance measured by the right side encoder.
   * @return The resulting Twist2d.
   */
  public Twist2d toTwist2d(double leftDistanceMeters, double rightDistanceMeters) {
    return new Twist2d(
        (leftDistanceMeters + rightDistanceMeters) / 2,
        0,
        (rightDistanceMeters - leftDistanceMeters) / trackWidthMeters);
  }

  @Override
  public DifferentialDriveWheelPositions copy(DifferentialDriveWheelPositions positions) {
    return new DifferentialDriveWheelPositions(positions.leftMeters, positions.rightMeters);
  }

  @Override
  public void copyInto(
      DifferentialDriveWheelPositions positions, DifferentialDriveWheelPositions output) {
    output.leftMeters = positions.leftMeters;
    output.rightMeters = positions.rightMeters;
  }

  @Override
  public DifferentialDriveWheelPositions interpolate(
      DifferentialDriveWheelPositions startValue,
      DifferentialDriveWheelPositions endValue,
      double t) {
    return startValue.interpolate(endValue, t);
  }
}
