// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.wpilib.units.Units.MetersPerSecond;
import static org.wpilib.units.Units.Seconds;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.trajectory.proto.DifferentialSampleProto;
import org.wpilib.units.measure.LinearVelocity;
import org.wpilib.units.measure.Time;
import org.wpilib.util.struct.StructSerializable;

/** Represents a single sample in a differential drive trajectory. */
public class DifferentialSample extends TrajectorySample implements StructSerializable {
  public final double leftSpeed; // meters per second
  public final double rightSpeed; // meters per second

  /** Base proto for serialization. */
  public static final DifferentialSampleProto proto = new DifferentialSampleProto();

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
        timestamp.in(Seconds),
        pose,
        velocity,
        acceleration,
        leftSpeed.in(MetersPerSecond),
        rightSpeed.in(MetersPerSecond));
  }

  /**
   * Constructs a DifferentialSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start, in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   * @param kinematics The kinematics of the drivetrain.
   */
  public DifferentialSample(
      double timestamp,
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
  public DifferentialSample(TrajectorySample sample, double leftSpeed, double rightSpeed) {
    this(
        sample.timestamp, sample.pose, sample.velocity, sample.acceleration, leftSpeed, rightSpeed);
  }

  /**
   * Constructs a DifferentialSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param kinematics The kinematics of the drivetrain.
   */
  public DifferentialSample(TrajectorySample sample, DifferentialDriveKinematics kinematics) {
    this(sample.timestamp, sample.pose, sample.velocity, sample.acceleration, kinematics);
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
  public DifferentialSample withNewTimestamp(double timestamp) {
    return new DifferentialSample(timestamp, pose, velocity, acceleration, leftSpeed, rightSpeed);
  }
}
