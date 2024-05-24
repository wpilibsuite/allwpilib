// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.controller.ProfiledPIDController;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveOdometry;
import edu.wpi.first.math.kinematics.MecanumDriveWheelPositions;
import edu.wpi.first.math.kinematics.MecanumDriveWheelSpeeds;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import java.util.ArrayList;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

class MecanumControllerCommandTest {
  @BeforeEach
  void setupAll() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
  }

  @AfterEach
  void cleanupAll() {
    SimHooks.resumeTiming();
  }

  private final Timer m_timer = new Timer();
  private Rotation2d m_angle = Rotation2d.kZero;

  private double m_frontLeftSpeed;
  private double m_frontLeftDistance;
  private double m_rearLeftSpeed;
  private double m_rearLeftDistance;
  private double m_frontRightSpeed;
  private double m_frontRightDistance;
  private double m_rearRightSpeed;
  private double m_rearRightDistance;

  private final ProfiledPIDController m_rotController =
      new ProfiledPIDController(1, 0, 0, new TrapezoidProfile.Constraints(3 * Math.PI, Math.PI));

  private static final double kxTolerance = 1 / 12.0;
  private static final double kyTolerance = 1 / 12.0;
  private static final double kAngularTolerance = 1 / 12.0;

  private static final double kWheelBase = 0.5;
  private static final double kTrackWidth = 0.5;

  private final MecanumDriveKinematics m_kinematics =
      new MecanumDriveKinematics(
          new Translation2d(kWheelBase / 2, kTrackWidth / 2),
          new Translation2d(kWheelBase / 2, -kTrackWidth / 2),
          new Translation2d(-kWheelBase / 2, kTrackWidth / 2),
          new Translation2d(-kWheelBase / 2, -kTrackWidth / 2));

  private final MecanumDriveOdometry m_odometry =
      new MecanumDriveOdometry(
          m_kinematics, Rotation2d.kZero, new MecanumDriveWheelPositions(), Pose2d.kZero);

  public void setWheelSpeeds(MecanumDriveWheelSpeeds wheelSpeeds) {
    this.m_frontLeftSpeed = wheelSpeeds.frontLeftMetersPerSecond;
    this.m_rearLeftSpeed = wheelSpeeds.rearLeftMetersPerSecond;
    this.m_frontRightSpeed = wheelSpeeds.frontRightMetersPerSecond;
    this.m_rearRightSpeed = wheelSpeeds.rearRightMetersPerSecond;
  }

  public MecanumDriveWheelPositions getCurrentWheelDistances() {
    return new MecanumDriveWheelPositions(
        m_frontLeftDistance, m_frontRightDistance, m_rearLeftDistance, m_rearRightDistance);
  }

  public Pose2d getRobotPose() {
    m_odometry.update(m_angle, getCurrentWheelDistances());
    return m_odometry.getPoseMeters();
  }

  @Test
  @ResourceLock("timing")
  void testReachesReference() {
    final var subsystem = new Subsystem() {};

    final var waypoints = new ArrayList<Pose2d>();
    waypoints.add(Pose2d.kZero);
    waypoints.add(new Pose2d(1, 5, new Rotation2d(3)));
    var config = new TrajectoryConfig(8.8, 0.1);
    final var trajectory = TrajectoryGenerator.generateTrajectory(waypoints, config);

    final var endState = trajectory.sample(trajectory.getTotalTimeSeconds());

    final var command =
        new MecanumControllerCommand(
            trajectory,
            this::getRobotPose,
            m_kinematics,
            new PIDController(0.6, 0, 0),
            new PIDController(0.6, 0, 0),
            m_rotController,
            8.8,
            this::setWheelSpeeds,
            subsystem);

    m_timer.restart();

    command.initialize();
    while (!command.isFinished()) {
      command.execute();
      m_angle = trajectory.sample(m_timer.get()).poseMeters.getRotation();
      m_frontLeftDistance += m_frontLeftSpeed * 0.005;
      m_rearLeftDistance += m_rearLeftSpeed * 0.005;
      m_frontRightDistance += m_frontRightSpeed * 0.005;
      m_rearRightDistance += m_rearRightSpeed * 0.005;
      SimHooks.stepTiming(0.005);
    }
    m_timer.stop();
    command.end(true);

    assertAll(
        () -> assertEquals(endState.poseMeters.getX(), getRobotPose().getX(), kxTolerance),
        () -> assertEquals(endState.poseMeters.getY(), getRobotPose().getY(), kyTolerance),
        () ->
            assertEquals(
                endState.poseMeters.getRotation().getRadians(),
                getRobotPose().getRotation().getRadians(),
                kAngularTolerance));
  }
}
