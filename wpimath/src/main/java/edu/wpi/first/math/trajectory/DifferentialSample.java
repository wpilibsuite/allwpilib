// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.MetersPerSecond;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
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
