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
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveDriveOdometry;
import edu.wpi.first.math.kinematics.SwerveModulePosition;
import edu.wpi.first.math.kinematics.SwerveModuleState;
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

class SwerveControllerCommandTest {
  @BeforeEach
  void setup() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
  }

  @AfterEach
  void cleanup() {
    SimHooks.resumeTiming();
  }

  private final Timer m_timer = new Timer();
  private Rotation2d m_angle = Rotation2d.kZero;

  private SwerveModuleState[] m_moduleStates =
      new SwerveModuleState[] {
        new SwerveModuleState(0, Rotation2d.kZero),
        new SwerveModuleState(0, Rotation2d.kZero),
        new SwerveModuleState(0, Rotation2d.kZero),
        new SwerveModuleState(0, Rotation2d.kZero)
      };

  private final SwerveModulePosition[] m_modulePositions =
      new SwerveModulePosition[] {
        new SwerveModulePosition(0, Rotation2d.kZero),
        new SwerveModulePosition(0, Rotation2d.kZero),
        new SwerveModulePosition(0, Rotation2d.kZero),
        new SwerveModulePosition(0, Rotation2d.kZero)
      };

  private final ProfiledPIDController m_rotController =
      new ProfiledPIDController(1, 0, 0, new TrapezoidProfile.Constraints(3 * Math.PI, Math.PI));

  private static final double kxTolerance = 1 / 12.0;
  private static final double kyTolerance = 1 / 12.0;
  private static final double kAngularTolerance = 1 / 12.0;

  private static final double kWheelBase = 0.5;
  private static final double kTrackWidth = 0.5;

  private final SwerveDriveKinematics m_kinematics =
      new SwerveDriveKinematics(
          new Translation2d(kWheelBase / 2, kTrackWidth / 2),
          new Translation2d(kWheelBase / 2, -kTrackWidth / 2),
          new Translation2d(-kWheelBase / 2, kTrackWidth / 2),
          new Translation2d(-kWheelBase / 2, -kTrackWidth / 2));

  private final SwerveDriveOdometry m_odometry =
      new SwerveDriveOdometry(m_kinematics, Rotation2d.kZero, m_modulePositions, Pose2d.kZero);

  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  public void setModuleStates(SwerveModuleState[] moduleStates) {
    this.m_moduleStates = moduleStates;
  }

  public Pose2d getRobotPose() {
    m_odometry.update(m_angle, m_modulePositions);
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
        new SwerveControllerCommand(
            trajectory,
            this::getRobotPose,
            m_kinematics,
            new PIDController(0.6, 0, 0),
            new PIDController(0.6, 0, 0),
            m_rotController,
            this::setModuleStates,
            subsystem);

    m_timer.restart();

    command.initialize();
    while (!command.isFinished()) {
      command.execute();
      m_angle = trajectory.sample(m_timer.get()).poseMeters.getRotation();

      for (int i = 0; i < m_modulePositions.length; i++) {
        m_modulePositions[i].distanceMeters += m_moduleStates[i].speedMetersPerSecond * 0.005;
        m_modulePositions[i].angle = m_moduleStates[i].angle;
      }

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
