// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import edu.wpi.first.math.util.Units;
import java.util.List;
import java.util.Random;
import org.junit.jupiter.api.Test;

class MecanumDriveOdometry3dTest {
  private final Translation2d m_fl = new Translation2d(12, 12);
  private final Translation2d m_fr = new Translation2d(12, -12);
  private final Translation2d m_bl = new Translation2d(-12, 12);
  private final Translation2d m_br = new Translation2d(-12, -12);

  private final MecanumDriveKinematics m_kinematics =
      new MecanumDriveKinematics(m_fl, m_fr, m_bl, m_br);

  private final MecanumDriveWheelPositions zero = new MecanumDriveWheelPositions();

  private final MecanumDriveOdometry3d m_odometry =
      new MecanumDriveOdometry3d(m_kinematics, Rotation3d.kZero, zero);

  @Test
  void testInitialize() {
    MecanumDriveOdometry3d odometry =
        new MecanumDriveOdometry3d(
            m_kinematics,
            Rotation3d.kZero,
            zero,
            new Pose3d(1, 2, 0, new Rotation3d(0, 0, Units.degreesToRadians(45))));
    var pose = odometry.getPoseMeters();
    assertAll(
        () -> assertEquals(pose.getX(), 1.0, 1e-9),
        () -> assertEquals(pose.getY(), 2.0, 1e-9),
        () -> assertEquals(pose.getZ(), 0.0, 1e-9),
        () -> assertEquals(pose.getRotation().toRotation2d().getDegrees(), 45.0, 1e-9));
  }

  @Test
  void testMultipleConsecutiveUpdates() {
    var wheelPositions = new MecanumDriveWheelPositions(3.536, 3.536, 3.536, 3.536);

    m_odometry.resetPosition(Rotation3d.kZero, wheelPositions, Pose3d.kZero);

    m_odometry.update(Rotation3d.kZero, wheelPositions);
    var secondPose = m_odometry.update(Rotation3d.kZero, wheelPositions);

    assertAll(
        () -> assertEquals(secondPose.getX(), 0.0, 0.01),
        () -> assertEquals(secondPose.getY(), 0.0, 0.01),
        () -> assertEquals(secondPose.getZ(), 0.0, 0.01),
        () -> assertEquals(secondPose.getRotation().toRotation2d().getDegrees(), 0.0, 0.01));
  }

  @Test
  void testTwoIterations() {
    // 5 units/sec  in the x-axis (forward)
    final var wheelPositions = new MecanumDriveWheelPositions(0.3536, 0.3536, 0.3536, 0.3536);
    m_odometry.resetPosition(Rotation3d.kZero, new MecanumDriveWheelPositions(), Pose3d.kZero);

    m_odometry.update(Rotation3d.kZero, new MecanumDriveWheelPositions());
    var pose = m_odometry.update(Rotation3d.kZero, wheelPositions);

    assertAll(
        () -> assertEquals(0.3536, pose.getX(), 0.01),
        () -> assertEquals(0.0, pose.getY(), 0.01),
        () -> assertEquals(0.0, pose.getZ(), 0.01),
        () -> assertEquals(0.0, pose.getRotation().toRotation2d().getDegrees(), 0.01));
  }

  @Test
  void test90degreeTurn() {
    // This is a 90 degree turn about the point between front left and rear left wheels
    // fl -13.328649 fr 39.985946 rl -13.328649 rr 39.985946
    final var wheelPositions = new MecanumDriveWheelPositions(-13.328, 39.986, -13.329, 39.986);
    m_odometry.resetPosition(Rotation3d.kZero, new MecanumDriveWheelPositions(), Pose3d.kZero);

    m_odometry.update(Rotation3d.kZero, new MecanumDriveWheelPositions());
    final var pose = m_odometry.update(new Rotation3d(0, 0, Math.PI / 2), wheelPositions);

    assertAll(
        () -> assertEquals(8.4855, pose.getX(), 0.01),
        () -> assertEquals(8.4855, pose.getY(), 0.01),
        () -> assertEquals(0.0, pose.getZ(), 0.01),
        () -> assertEquals(90.0, pose.getRotation().toRotation2d().getDegrees(), 0.01));
  }

  @Test
  void testGyroAngleReset() {
    var gyro = new Rotation3d(0, 0, Math.PI / 2);
    var fieldAngle = Rotation3d.kZero;
    m_odometry.resetPosition(
        gyro, new MecanumDriveWheelPositions(), new Pose3d(Translation3d.kZero, fieldAngle));
    var speeds = new MecanumDriveWheelPositions(3.536, 3.536, 3.536, 3.536);
    m_odometry.update(gyro, new MecanumDriveWheelPositions());
    var pose = m_odometry.update(gyro, speeds);

    assertAll(
        () -> assertEquals(3.536, pose.getX(), 0.1),
        () -> assertEquals(0.0, pose.getY(), 0.1),
        () -> assertEquals(0.0, pose.getZ(), 0.1),
        () -> assertEquals(0.0, pose.getRotation().toRotation2d().getRadians(), 0.1));
  }

  @Test
  void testAccuracyFacingTrajectory() {
    var kinematics =
        new MecanumDriveKinematics(
            new Translation2d(1, 1), new Translation2d(1, -1),
            new Translation2d(-1, -1), new Translation2d(-1, 1));

    var wheelPositions = new MecanumDriveWheelPositions();

    var odometry =
        new MecanumDriveOdometry3d(kinematics, Rotation3d.kZero, wheelPositions, Pose3d.kZero);

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                Pose2d.kZero,
                new Pose2d(20, 20, Rotation2d.kZero),
                new Pose2d(10, 10, Rotation2d.kPi),
                new Pose2d(30, 30, Rotation2d.kZero),
                new Pose2d(20, 20, Rotation2d.kPi),
                new Pose2d(10, 10, Rotation2d.kZero)),
            new TrajectoryConfig(0.5, 2));

    var rand = new Random(5190);

    final double dt = 0.02;
    double t = 0.0;

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    double odometryDistanceTravelled = 0;
    double trajectoryDistanceTravelled = 0;
    while (t <= trajectory.getTotalTimeSeconds()) {
      var groundTruthState = trajectory.sample(t);

      trajectoryDistanceTravelled +=
          groundTruthState.velocityMetersPerSecond * dt
              + 0.5 * groundTruthState.accelerationMetersPerSecondSq * dt * dt;

      var wheelSpeeds =
          kinematics.toWheelSpeeds(
              new ChassisSpeeds(
                  groundTruthState.velocityMetersPerSecond,
                  0,
                  groundTruthState.velocityMetersPerSecond
                      * groundTruthState.curvatureRadPerMeter));

      wheelSpeeds.frontLeftMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.frontRightMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.rearLeftMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.rearRightMetersPerSecond += rand.nextGaussian() * 0.1;

      wheelPositions.frontLeftMeters += wheelSpeeds.frontLeftMetersPerSecond * dt;
      wheelPositions.frontRightMeters += wheelSpeeds.frontRightMetersPerSecond * dt;
      wheelPositions.rearLeftMeters += wheelSpeeds.rearLeftMetersPerSecond * dt;
      wheelPositions.rearRightMeters += wheelSpeeds.rearRightMetersPerSecond * dt;

      var lastPose = odometry.getPoseMeters();

      var xHat =
          odometry.update(
              new Rotation3d(
                  groundTruthState
                      .poseMeters
                      .getRotation()
                      .plus(new Rotation2d(rand.nextGaussian() * 0.05))),
              wheelPositions);

      odometryDistanceTravelled += lastPose.getTranslation().getDistance(xHat.getTranslation());

      double error =
          groundTruthState
              .poseMeters
              .getTranslation()
              .getDistance(xHat.getTranslation().toTranslation2d());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(
        0.0, errorSum / (trajectory.getTotalTimeSeconds() / dt), 0.35, "Incorrect mean error");
    assertEquals(0.0, maxError, 0.35, "Incorrect max error");
    assertEquals(
        1.0,
        odometryDistanceTravelled / trajectoryDistanceTravelled,
        0.05,
        "Incorrect distance travelled");
  }

  @Test
  void testAccuracyFacingXAxis() {
    var kinematics =
        new MecanumDriveKinematics(
            new Translation2d(1, 1), new Translation2d(1, -1),
            new Translation2d(-1, -1), new Translation2d(-1, 1));

    var wheelPositions = new MecanumDriveWheelPositions();

    var odometry =
        new MecanumDriveOdometry3d(kinematics, Rotation3d.kZero, wheelPositions, Pose3d.kZero);

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                Pose2d.kZero,
                new Pose2d(20, 20, Rotation2d.kZero),
                new Pose2d(10, 10, Rotation2d.kPi),
                new Pose2d(30, 30, Rotation2d.kZero),
                new Pose2d(20, 20, Rotation2d.kPi),
                new Pose2d(10, 10, Rotation2d.kZero)),
            new TrajectoryConfig(0.5, 2));

    var rand = new Random(5190);

    final double dt = 0.02;
    double t = 0.0;

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    double odometryDistanceTravelled = 0;
    double trajectoryDistanceTravelled = 0;
    while (t <= trajectory.getTotalTimeSeconds()) {
      var groundTruthState = trajectory.sample(t);

      trajectoryDistanceTravelled +=
          groundTruthState.velocityMetersPerSecond * dt
              + 0.5 * groundTruthState.accelerationMetersPerSecondSq * dt * dt;

      var wheelSpeeds =
          kinematics.toWheelSpeeds(
              new ChassisSpeeds(
                  groundTruthState.velocityMetersPerSecond
                      * groundTruthState.poseMeters.getRotation().getCos(),
                  groundTruthState.velocityMetersPerSecond
                      * groundTruthState.poseMeters.getRotation().getSin(),
                  0));

      wheelSpeeds.frontLeftMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.frontRightMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.rearLeftMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.rearRightMetersPerSecond += rand.nextGaussian() * 0.1;

      wheelPositions.frontLeftMeters += wheelSpeeds.frontLeftMetersPerSecond * dt;
      wheelPositions.frontRightMeters += wheelSpeeds.frontRightMetersPerSecond * dt;
      wheelPositions.rearLeftMeters += wheelSpeeds.rearLeftMetersPerSecond * dt;
      wheelPositions.rearRightMeters += wheelSpeeds.rearRightMetersPerSecond * dt;

      var lastPose = odometry.getPoseMeters();

      var xHat = odometry.update(new Rotation3d(0, 0, rand.nextGaussian() * 0.05), wheelPositions);

      odometryDistanceTravelled += lastPose.getTranslation().getDistance(xHat.getTranslation());

      double error =
          groundTruthState
              .poseMeters
              .getTranslation()
              .getDistance(xHat.getTranslation().toTranslation2d());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(
        0.0, errorSum / (trajectory.getTotalTimeSeconds() / dt), 0.15, "Incorrect mean error");
    assertEquals(0.0, maxError, 0.3, "Incorrect max error");
    assertEquals(
        1.0,
        odometryDistanceTravelled / trajectoryDistanceTravelled,
        0.05,
        "Incorrect distance travelled");
  }
}
