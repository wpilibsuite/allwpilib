// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import java.util.List;
import java.util.Random;
import org.junit.jupiter.api.Test;

class SwerveDriveOdometryTest {
  private final Translation2d m_fl = new Translation2d(12, 12);
  private final Translation2d m_fr = new Translation2d(12, -12);
  private final Translation2d m_bl = new Translation2d(-12, 12);
  private final Translation2d m_br = new Translation2d(-12, -12);

  private final SwerveModulePosition zero = new SwerveModulePosition();

  private final SwerveDriveKinematics m_kinematics =
      new SwerveDriveKinematics(m_fl, m_fr, m_bl, m_br);

  private final SwerveDriveOdometry m_odometry =
      new SwerveDriveOdometry(
          m_kinematics, Rotation2d.kZero, new SwerveModulePosition[] {zero, zero, zero, zero});

  @Test
  void testTwoIterations() {
    // 5 units/sec  in the x-axis (forward)
    final SwerveModulePosition[] wheelDeltas = {
      new SwerveModulePosition(0.5, Rotation2d.kZero),
      new SwerveModulePosition(0.5, Rotation2d.kZero),
      new SwerveModulePosition(0.5, Rotation2d.kZero),
      new SwerveModulePosition(0.5, Rotation2d.kZero)
    };

    m_odometry.update(
        Rotation2d.kZero,
        new SwerveModulePosition[] {
          new SwerveModulePosition(),
          new SwerveModulePosition(),
          new SwerveModulePosition(),
          new SwerveModulePosition()
        });
    var pose = m_odometry.update(Rotation2d.kZero, wheelDeltas);

    assertAll(
        () -> assertEquals(5.0 / 10.0, pose.getX(), 0.01),
        () -> assertEquals(0, pose.getY(), 0.01),
        () -> assertEquals(0.0, pose.getRotation().getDegrees(), 0.01));
  }

  @Test
  void test90degreeTurn() {
    // This is a 90 degree turn about the point between front left and rear left wheels
    //        Module 0: speed 18.84955592153876 angle 90.0
    //        Module 1: speed 42.14888838624436 angle 26.565051177077986
    //        Module 2: speed 18.84955592153876 angle -90.0
    //        Module 3: speed 42.14888838624436 angle -26.565051177077986

    final SwerveModulePosition[] wheelDeltas = {
      new SwerveModulePosition(18.85, Rotation2d.kCCW_Pi_2),
      new SwerveModulePosition(42.15, Rotation2d.fromDegrees(26.565)),
      new SwerveModulePosition(18.85, Rotation2d.kCW_Pi_2),
      new SwerveModulePosition(42.15, Rotation2d.fromDegrees(-26.565))
    };
    final var zero = new SwerveModulePosition();

    m_odometry.update(Rotation2d.kZero, new SwerveModulePosition[] {zero, zero, zero, zero});
    final var pose = m_odometry.update(Rotation2d.kCCW_Pi_2, wheelDeltas);

    assertAll(
        () -> assertEquals(12.0, pose.getX(), 0.01),
        () -> assertEquals(12.0, pose.getY(), 0.01),
        () -> assertEquals(90.0, pose.getRotation().getDegrees(), 0.01));
  }

  @Test
  void testGyroAngleReset() {
    var gyro = Rotation2d.kCCW_Pi_2;
    var fieldAngle = Rotation2d.kZero;
    m_odometry.resetPosition(
        gyro,
        new SwerveModulePosition[] {zero, zero, zero, zero},
        new Pose2d(Translation2d.kZero, fieldAngle));
    var delta = new SwerveModulePosition();
    m_odometry.update(gyro, new SwerveModulePosition[] {delta, delta, delta, delta});
    delta = new SwerveModulePosition(1.0, Rotation2d.kZero);
    var pose = m_odometry.update(gyro, new SwerveModulePosition[] {delta, delta, delta, delta});

    assertAll(
        () -> assertEquals(1.0, pose.getX(), 0.1),
        () -> assertEquals(0.00, pose.getY(), 0.1),
        () -> assertEquals(0.00, pose.getRotation().getRadians(), 0.1));
  }

  @Test
  void testAccuracyFacingTrajectory() {
    var kinematics =
        new SwerveDriveKinematics(
            new Translation2d(1, 1),
            new Translation2d(1, -1),
            new Translation2d(-1, -1),
            new Translation2d(-1, 1));
    var odometry =
        new SwerveDriveOdometry(
            kinematics, Rotation2d.kZero, new SwerveModulePosition[] {zero, zero, zero, zero});

    SwerveModulePosition fl = new SwerveModulePosition();
    SwerveModulePosition fr = new SwerveModulePosition();
    SwerveModulePosition bl = new SwerveModulePosition();
    SwerveModulePosition br = new SwerveModulePosition();

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                new Pose2d(3, 0, Rotation2d.kCW_Pi_2),
                new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                new Pose2d(-3, 0, Rotation2d.kCW_Pi_2),
                new Pose2d(0, 0, Rotation2d.fromDegrees(45))),
            new TrajectoryConfig(0.5, 2));

    var rand = new Random(4915);

    final double dt = 0.02;
    double t = 0.0;

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    while (t <= trajectory.getTotalTime()) {
      var groundTruthState = trajectory.sample(t);

      var moduleStates =
          kinematics.toSwerveModuleStates(
              new ChassisSpeeds(
                  groundTruthState.velocity,
                  0.0,
                  groundTruthState.velocity * groundTruthState.curvature));
      for (var moduleState : moduleStates) {
        moduleState.angle = moduleState.angle.plus(new Rotation2d(rand.nextGaussian() * 0.005));
        moduleState.speed += rand.nextGaussian() * 0.1;
      }

      fl.distance += moduleStates[0].speed * dt;
      fr.distance += moduleStates[1].speed * dt;
      bl.distance += moduleStates[2].speed * dt;
      br.distance += moduleStates[3].speed * dt;

      fl.angle = moduleStates[0].angle;
      fr.angle = moduleStates[1].angle;
      bl.angle = moduleStates[2].angle;
      br.angle = moduleStates[3].angle;

      var xHat =
          odometry.update(
              groundTruthState.pose.getRotation().plus(new Rotation2d(rand.nextGaussian() * 0.05)),
              new SwerveModulePosition[] {fl, fr, bl, br});

      double error = groundTruthState.pose.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(0.0, odometry.getPose().getX(), 1e-1, "Incorrect Final X");
    assertEquals(0.0, odometry.getPose().getY(), 1e-1, "Incorrect Final Y");
    assertEquals(
        Math.PI / 4,
        odometry.getPose().getRotation().getRadians(),
        10 * Math.PI / 180,
        "Incorrect Final Theta");

    assertEquals(0.0, errorSum / (trajectory.getTotalTime() / dt), 0.05, "Incorrect mean error");
    assertEquals(0.0, maxError, 0.125, "Incorrect max error");
  }

  @Test
  void testAccuracyFacingXAxis() {
    var kinematics =
        new SwerveDriveKinematics(
            new Translation2d(1, 1),
            new Translation2d(1, -1),
            new Translation2d(-1, -1),
            new Translation2d(-1, 1));
    var odometry =
        new SwerveDriveOdometry(
            kinematics, Rotation2d.kZero, new SwerveModulePosition[] {zero, zero, zero, zero});

    SwerveModulePosition fl = new SwerveModulePosition();
    SwerveModulePosition fr = new SwerveModulePosition();
    SwerveModulePosition bl = new SwerveModulePosition();
    SwerveModulePosition br = new SwerveModulePosition();

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                new Pose2d(3, 0, Rotation2d.kCW_Pi_2),
                new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                new Pose2d(-3, 0, Rotation2d.kCW_Pi_2),
                new Pose2d(0, 0, Rotation2d.fromDegrees(45))),
            new TrajectoryConfig(0.5, 2));

    var rand = new Random(4915);

    final double dt = 0.02;
    double t = 0.0;

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    while (t <= trajectory.getTotalTime()) {
      var groundTruthState = trajectory.sample(t);

      fl.distance += groundTruthState.velocity * dt + 0.5 * groundTruthState.acceleration * dt * dt;
      fr.distance += groundTruthState.velocity * dt + 0.5 * groundTruthState.acceleration * dt * dt;
      bl.distance += groundTruthState.velocity * dt + 0.5 * groundTruthState.acceleration * dt * dt;
      br.distance += groundTruthState.velocity * dt + 0.5 * groundTruthState.acceleration * dt * dt;

      fl.angle = groundTruthState.pose.getRotation();
      fr.angle = groundTruthState.pose.getRotation();
      bl.angle = groundTruthState.pose.getRotation();
      br.angle = groundTruthState.pose.getRotation();

      var xHat =
          odometry.update(
              new Rotation2d(rand.nextGaussian() * 0.05),
              new SwerveModulePosition[] {fl, fr, bl, br});

      double error = groundTruthState.pose.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(0.0, odometry.getPose().getX(), 1e-1, "Incorrect Final X");
    assertEquals(0.0, odometry.getPose().getY(), 1e-1, "Incorrect Final Y");
    assertEquals(
        0.0,
        odometry.getPose().getRotation().getRadians(),
        10 * Math.PI / 180,
        "Incorrect Final Theta");

    assertEquals(0.0, errorSum / (trajectory.getTotalTime() / dt), 0.06, "Incorrect mean error");
    assertEquals(0.0, maxError, 0.125, "Incorrect max error");
  }
}
