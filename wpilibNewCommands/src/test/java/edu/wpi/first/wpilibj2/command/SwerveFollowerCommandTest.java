/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import java.util.ArrayList;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.controller.ProfiledPIDController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveOdometry;
import edu.wpi.first.wpilibj.kinematics.SwerveModuleState;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class SwerveFollowerCommandTest {
  private final Timer m_rotTimer = new Timer();
  private Rotation2d m_angle = new Rotation2d(0);

  private SwerveModuleState[] m_moduleStates = new SwerveModuleState[]{
    new SwerveModuleState(0, new Rotation2d(0)),
    new SwerveModuleState(0, new Rotation2d(0)),
    new SwerveModuleState(0, new Rotation2d(0)),
    new SwerveModuleState(0, new Rotation2d(0))};

  private final ProfiledPIDController m_rotController = new ProfiledPIDController(1, 0, 0,
      new TrapezoidProfile.Constraints(3 * Math.PI, Math.PI));

  private static final double kxTolerance = 1 / 12.0;
  private static final double kyTolerance = 1 / 12.0;

  private static final double m_trackLength = 0.5;
  private static final double m_trackWidth = 0.5;

  private final SwerveDriveKinematics m_kinematics = new SwerveDriveKinematics(
      new Translation2d(m_trackLength / 2, m_trackWidth / 2),
      new Translation2d(m_trackLength / 2, -m_trackWidth / 2),
      new Translation2d(-m_trackLength / 2, m_trackWidth / 2),
      new Translation2d(-m_trackLength / 2, -m_trackWidth / 2));

  private final SwerveDriveOdometry m_odometry = new SwerveDriveOdometry(m_kinematics,
      new Rotation2d(0), new Pose2d(0, 0, new Rotation2d(0)));

  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  public void setModuleStates(SwerveModuleState[] moduleStates) {
    this.m_moduleStates = moduleStates;
  }

  public Pose2d getRobotPose() {
    m_odometry.update(m_angle, m_moduleStates);
    return m_odometry.getPoseMeters();
  }

  @Test
  @SuppressWarnings("PMD.AvoidInstantiatingObjectsInLoops")
  void testReachesReference() {
    final var subsystem = new Subsystem() {};

    final var waypoints = new ArrayList<Pose2d>();
    waypoints.add(new Pose2d(0, 0, new Rotation2d(0)));
    waypoints.add(new Pose2d(1, 5, new Rotation2d(3)));
    var config = new TrajectoryConfig(8.8, 0.1);
    final var trajectory = TrajectoryGenerator.generateTrajectory(waypoints, config);

    final var endState = trajectory.sample(trajectory.getTotalTimeSeconds());

    final var command = new SwerveFollowerCommand(trajectory,
        this::getRobotPose,
        m_kinematics,
        new PIDController(0.6, 0, 0),
        new PIDController(0.6, 0, 0),
        m_rotController,
        this::setModuleStates,
        subsystem);

    m_rotTimer.reset();
    m_rotTimer.start();

    command.initialize();
    while (!command.isFinished()) {
      command.execute();
      m_angle = trajectory.sample(m_rotTimer.get()).poseMeters.getRotation();
    }
    m_rotTimer.stop();
    command.end(true);

    assertAll(
        () -> assertEquals(endState.poseMeters.getTranslation().getX(),
          getRobotPose().getTranslation().getX(), kxTolerance),
        () -> assertEquals(endState.poseMeters.getTranslation().getY(),
          getRobotPose().getTranslation().getY(), kyTolerance),
        () -> assertEquals(endState.poseMeters.getRotation().getRadians(),
          getRobotPose().getRotation().getRadians())
    );
  }
}
