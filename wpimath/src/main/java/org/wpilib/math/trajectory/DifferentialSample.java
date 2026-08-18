// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.wpilib.units.Units.MetersPerSecond;
import static org.wpilib.units.Units.Seconds;

import io.avaje.jsonb.Json;
import java.util.Objects;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.trajectory.proto.DifferentialSampleProto;
import org.wpilib.math.trajectory.struct.DifferentialSampleStruct;
import org.wpilib.units.measure.LinearVelocity;
import org.wpilib.units.measure.Time;
import org.wpilib.util.struct.StructSerializable;

/** Represents a single sample in a differential drive trajectory. */
@Json
public class DifferentialSample extends HolonomicSample implements StructSerializable {
  /** The left-wheel velocity at this sample in meters per second. */
  @Json.Property("leftVelocity")
  public double leftVelocity; // meters per second

  /** The right-wheel velocity at this sample in meters per second. */
  @Json.Property("rightVelocity")
  public double rightVelocity; // meters per second

  /** Base proto for serialization. */
  public static final DifferentialSampleProto proto = new DifferentialSampleProto();

  /** Base struct for serialization. */
  public static final DifferentialSampleStruct struct = new DifferentialSampleStruct();

  /**
   * Constructs a DifferentialSample.
   *
   * @param time The time of the sample relative to the trajectory start, in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the field reference frame).
   * @param acceleration The robot acceleration at this sample (in the field reference frame).
   * @param leftVelocity The left-wheel velocity at this sample in meters per second.
   * @param rightVelocity The right-wheel velocity at this sample in meters per second.
   */
  @Json.Creator
  public DifferentialSample(
      double time,
      Pose2d pose,
      ChassisVelocities velocity,
      ChassisAccelerations acceleration,
      double leftVelocity,
      double rightVelocity) {
    super(time, pose, velocity, acceleration);

    this.leftVelocity = leftVelocity;
    this.rightVelocity = rightVelocity;
  }

  /**
   * Constructs a DifferentialSample.
   *
   * @param time The time of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the field reference frame).
   * @param acceleration The robot acceleration at this sample (in the field reference frame).
   * @param leftVelocity The left wheel velocity at this sample.
   * @param rightVelocity The right wheel velocity at this sample.
   */
  public DifferentialSample(
      Time time,
      Pose2d pose,
      ChassisVelocities velocity,
      ChassisAccelerations acceleration,
      LinearVelocity leftVelocity,
      LinearVelocity rightVelocity) {
    this(
        time.in(Seconds),
        pose,
        velocity,
        acceleration,
        leftVelocity.in(MetersPerSecond),
        rightVelocity.in(MetersPerSecond));
  }

  /**
   * Constructs a DifferentialSample.
   *
   * @param time The time of the sample relative to the trajectory start, in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the field reference frame).
   * @param acceleration The robot acceleration at this sample (in the field reference frame).
   * @param kinematics The kinematics of the drivetrain.
   */
  public DifferentialSample(
      double time,
      Pose2d pose,
      ChassisVelocities velocity,
      ChassisAccelerations acceleration,
      DifferentialDriveKinematics kinematics) {
    super(time, pose, velocity, acceleration);

    // Wheel velocitys are derived from the robot-relative velocity.
    var wheelVelocitys = kinematics.toWheelVelocities(velocity.toRobotRelative(pose.getRotation()));
    this.leftVelocity = wheelVelocitys.left;
    this.rightVelocity = wheelVelocitys.right;
  }

  /**
   * Constructs a DifferentialSample from a HolonomicSample.
   *
   * @param sample The HolonomicSample to copy.
   * @param leftVelocity The left wheel velocity at this sample in meters per second.
   * @param rightVelocity The right wheel velocity at this sample in meters per second.
   */
  public DifferentialSample(HolonomicSample sample, double leftVelocity, double rightVelocity) {
    this(
        sample.time,
        sample.pose,
        sample.velocity,
        sample.acceleration,
        leftVelocity,
        rightVelocity);
  }

  /**
   * Constructs a DifferentialSample from a HolonomicSample.
   *
   * @param sample The HolonomicSample to copy.
   * @param kinematics The kinematics of the drivetrain.
   */
  public DifferentialSample(HolonomicSample sample, DifferentialDriveKinematics kinematics) {
    this(sample.time, sample.pose, sample.velocity, sample.acceleration, kinematics);
  }

  @Override
  public boolean equals(Object o) {
    if (!super.equals(o)) {
      return false;
    }

    DifferentialSample that = (DifferentialSample) o;
    return Double.compare(leftVelocity, that.leftVelocity) == 0
        && Double.compare(rightVelocity, that.rightVelocity) == 0;
  }

  @Override
  public int hashCode() {
    return Objects.hash(time, pose, velocity, acceleration, leftVelocity, rightVelocity);
  }

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  @Override
  public DifferentialSample transform(Transform2d transform) {
    return new DifferentialSample(
        time,
        pose.transformBy(transform),
        velocity.toFieldRelative(transform.getRotation()),
        acceleration.toFieldRelative(transform.getRotation()),
        leftVelocity,
        rightVelocity);
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  @Override
  public DifferentialSample relativeTo(Pose2d other) {
    return new DifferentialSample(
        time,
        pose.relativeTo(other),
        velocity.toRobotRelative(other.getRotation()),
        acceleration.toRobotRelative(other.getRotation()),
        leftVelocity,
        rightVelocity);
  }
}
