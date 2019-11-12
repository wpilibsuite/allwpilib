/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import java.util.ArrayList;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.controller.ProfiledPIDController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.geometry.Twist2d;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveOdometry;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveWheelSpeeds;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class MecanumFollowerCommandTest {
  private double frontLeftSpeed;
  private double rearLeftSpeed;
  private double frontRightSpeed;
  private double rearRightSpeed;

  private static double boundRadians(double value) {
    while (value > Math.PI) {
      value -= Math.PI * 2;
    }
    while (value <= -Math.PI) {
      value += Math.PI * 2;
    }
    return value;
  }

  public void setFrontLeftSpeed(double frontLeftSpeed) {
    this.frontLeftSpeed = frontLeftSpeed;
  }
  public void setRearLeftSpeed(double rearLeftSpeed) {
    this.rearLeftSpeed = rearLeftSpeed;
  }
  public void setFrontRightSpeed(double frontRightSpeed) {
    this.frontRightSpeed = frontRightSpeed;
  }
  public void setRearRightSpeed(double rearRightSpeed) {
    this.rearRightSpeed = rearRightSpeed;
  }

  public double getFrontLeftSpeed() {
    return frontLeftSpeed;
  }
  public double getRearLeftSpeed() {
    return rearLeftSpeed;
  }
  public double getFrontRightSpeed() {
    return frontRightSpeed;
  }
  public double getRearRightSpeed() {
    return rearRightSpeed;
  }

  @Test
  @SuppressWarnings("PMD.AvoidInstantiatingObjectsInLoops")
  void testReachesReference() {
    CommandScheduler scheduler = new CommandScheduler();

    final var subsystem = new Subsystem() {};

    final var kTrackLength = 0.7;
    final var kTrackWidth = 0.5;

    final var kinematics = new MecanumDriveKinematics(
      new Translation2d(kTrackLength / 2, kTrackWidth / 2),
      new Translation2d(kTrackLength / 2, -kTrackWidth / 2),
      new Translation2d(-kTrackLength / 2, kTrackWidth / 2),
      new Translation2d(-kTrackLength / 2, -kTrackWidth / 2));

    final var odometry = new MecanumDriveOdometry(kinematics, new Pose2d(0, 0, new Rotation2d(0)));

    final var waypoints = new ArrayList<Pose2d>();
    waypoints.add(new Pose2d(2.75, 22.521, new Rotation2d(0)));
    waypoints.add(new Pose2d(24.73, 19.68, new Rotation2d(5.846)));
    var config = new TrajectoryConfig(8.8, 0.1);
    final var trajectory = TrajectoryGenerator.generateTrajectory(waypoints, config);

    final double kDt = 0.02;
    final var totalTime = trajectory.getTotalTimeSeconds();

    final var command = new MecanumFollowerCommand(trajectory, 
      odometry::getPoseMeters,
      kinematics,
      new PIDController(1, 0, 0),
      new PIDController(1, 0, 0),
      new ProfiledPIDController(1, 0, 0,
        new TrapezoidProfile.Constraints(Math.PI, 3.0)),
      3.0,
      this::setFrontLeftSpeed,
      this::setRearLeftSpeed,
      this::setFrontRightSpeed,
      this::setRearRightSpeed,
      subsystem);
    
    scheduler.schedule(command);
    odometry.update(odometry.getPoseMeters().getRotation(), new MecanumDriveWheelSpeeds(getFrontLeftSpeed(), getFrontRightSpeed(), getRearLeftSpeed(), getRearRightSpeed()));
    scheduler.run();

    assertAll(
        () -> assertEquals(endPose.getTranslation().getX(), finalRobotPose.getTranslation().getX(),
            kTolerance),
        () -> assertEquals(endPose.getTranslation().getY(), finalRobotPose.getTranslation().getY(),
            kTolerance),
        () -> assertEquals(0.0,
            boundRadians(endPose.getRotation().getRadians()
                - finalRobotPose.getRotation().getRadians()),
            kAngularTolerance)
    );
  }
}

