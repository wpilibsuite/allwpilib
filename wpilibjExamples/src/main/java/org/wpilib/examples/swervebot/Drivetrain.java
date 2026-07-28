// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.swervebot;

import org.wpilib.hardware.imu.OnboardIMU;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.SwerveDriveKinematics;
import org.wpilib.math.kinematics.SwerveDriveOdometry;
import org.wpilib.math.kinematics.SwerveModulePosition;

/** Represents a swerve drive style drivetrain. */
public class Drivetrain {
  public static final double kMaxVelocity = 3.0; // 3 meters per second
  public static final double kMaxAngularVelocity = Math.PI; // 1/2 rotation per second

  private final Translation2d frontLeftLocation = new Translation2d(0.381, 0.381);
  private final Translation2d frontRightLocation = new Translation2d(0.381, -0.381);
  private final Translation2d backLeftLocation = new Translation2d(-0.381, 0.381);
  private final Translation2d backRightLocation = new Translation2d(-0.381, -0.381);

  private final SwerveModule frontLeft = new SwerveModule(1, 2, 0, 1, 2, 3);
  private final SwerveModule frontRight = new SwerveModule(3, 4, 4, 5, 6, 7);
  private final SwerveModule backLeft = new SwerveModule(5, 6, 8, 9, 10, 11);
  private final SwerveModule backRight = new SwerveModule(7, 8, 12, 13, 14, 15);

  private final OnboardIMU imu = new OnboardIMU(OnboardIMU.MountOrientation.FLAT);

  private final SwerveDriveKinematics kinematics =
      new SwerveDriveKinematics(
          frontLeftLocation, frontRightLocation, backLeftLocation, backRightLocation);

  private final SwerveDriveOdometry odometry =
      new SwerveDriveOdometry(
          kinematics,
          imu.getRotation2d(),
          new SwerveModulePosition[] {
            frontLeft.getPosition(),
            frontRight.getPosition(),
            backLeft.getPosition(),
            backRight.getPosition()
          });

  public Drivetrain() {
    imu.resetYaw();
  }

  /**
   * Method to drive the robot using joystick info.
   *
   * @param xVelocity Velocity of the robot in the x direction (forward).
   * @param yVelocity Velocity of the robot in the y direction (sideways).
   * @param rot Angular rate of the robot.
   * @param fieldRelative Whether the provided x and y velocities are relative to the field.
   */
  public void drive(
      double xVelocity, double yVelocity, double rot, boolean fieldRelative, double period) {
    var chassisVelocities = new ChassisVelocities(xVelocity, yVelocity, rot);
    if (fieldRelative) {
      chassisVelocities = chassisVelocities.toRobotRelative(imu.getRotation2d());
    }
    chassisVelocities = chassisVelocities.discretize(period);

    var velocities =
        SwerveDriveKinematics.desaturateWheelVelocities(
            kinematics.toWheelVelocities(chassisVelocities), kMaxVelocity);

    frontLeft.setDesiredVelocity(velocities[0]);
    frontRight.setDesiredVelocity(velocities[1]);
    backLeft.setDesiredVelocity(velocities[2]);
    backRight.setDesiredVelocity(velocities[3]);
  }

  /** Updates the field relative position of the robot. */
  public void updateOdometry() {
    odometry.update(
        imu.getRotation2d(),
        new SwerveModulePosition[] {
          frontLeft.getPosition(),
          frontRight.getPosition(),
          backLeft.getPosition(),
          backRight.getPosition()
        });
  }
}
