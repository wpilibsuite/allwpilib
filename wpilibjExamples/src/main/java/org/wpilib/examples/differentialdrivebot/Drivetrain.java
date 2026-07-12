// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.differentialdrivebot;

import org.wpilib.hardware.imu.OnboardIMU;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.kinematics.DifferentialDriveOdometry;
import org.wpilib.math.kinematics.DifferentialDriveWheelVelocities;

/** Represents a differential drive style drivetrain. */
public class Drivetrain {
  public static final double kMaxVelocity = 3.0; // meters per second
  public static final double kMaxAngularVelocity = 2 * Math.PI; // one rotation per second

  private static final double kTrackwidth = 0.381 * 2; // meters
  private static final double kWheelRadius = 0.0508; // meters
  private static final int kEncoderResolution = 4096;

  private final PWMSparkMax leftLeader = new PWMSparkMax(1);
  private final PWMSparkMax leftFollower = new PWMSparkMax(2);
  private final PWMSparkMax rightLeader = new PWMSparkMax(3);
  private final PWMSparkMax rightFollower = new PWMSparkMax(4);

  private final Encoder leftEncoder = new Encoder(0, 1);
  private final Encoder rightEncoder = new Encoder(2, 3);

  private final OnboardIMU imu = new OnboardIMU(OnboardIMU.MountOrientation.FLAT);

  private final PIDController leftPIDController = new PIDController(1, 0, 0);
  private final PIDController rightPIDController = new PIDController(1, 0, 0);

  private final DifferentialDriveKinematics kinematics =
      new DifferentialDriveKinematics(kTrackwidth);

  private final DifferentialDriveOdometry odometry;

  // Gains are for example purposes only - must be determined for your own robot!
  private final SimpleMotorFeedforward feedforward = new SimpleMotorFeedforward(1, 3);

  /**
   * Constructs a differential drive object. Sets the encoder distance per pulse and resets the
   * gyro.
   */
  public Drivetrain() {
    imu.resetYaw();

    leftLeader.addFollower(leftFollower);
    rightLeader.addFollower(rightFollower);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightLeader.setInverted(true);

    // Set the distance per pulse for the drive encoders. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    leftEncoder.setDistancePerPulse(2 * Math.PI * kWheelRadius / kEncoderResolution);
    rightEncoder.setDistancePerPulse(2 * Math.PI * kWheelRadius / kEncoderResolution);

    leftEncoder.reset();
    rightEncoder.reset();

    odometry =
        new DifferentialDriveOdometry(
            imu.getRotation2d(), leftEncoder.getDistance(), rightEncoder.getDistance());
  }

  /**
   * Sets the desired wheel velocities.
   *
   * @param velocities The desired wheel velocities.
   */
  public void setVelocities(DifferentialDriveWheelVelocities velocities) {
    final double leftFeedforward = feedforward.calculate(velocities.left);
    final double rightFeedforward = feedforward.calculate(velocities.right);

    final double leftOutput = leftPIDController.calculate(leftEncoder.getRate(), velocities.left);
    final double rightOutput =
        rightPIDController.calculate(rightEncoder.getRate(), velocities.right);
    leftLeader.setVoltage(leftOutput + leftFeedforward);
    rightLeader.setVoltage(rightOutput + rightFeedforward);
  }

  /**
   * Drives the robot with the given linear velocity and angular velocity.
   *
   * @param xVelocity Linear velocity in m/s.
   * @param rot Angular velocity in rad/s.
   */
  public void drive(double xVelocity, double rot) {
    var wheelVelocities = kinematics.toWheelVelocities(new ChassisVelocities(xVelocity, 0.0, rot));
    setVelocities(wheelVelocities);
  }

  /** Updates the field-relative position. */
  public void updateOdometry() {
    odometry.update(imu.getRotation2d(), leftEncoder.getDistance(), rightEncoder.getDistance());
  }
}
