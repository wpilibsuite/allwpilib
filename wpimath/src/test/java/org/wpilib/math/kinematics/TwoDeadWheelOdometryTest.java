// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.List;
import java.util.Random;
import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.trajectory.DrivetrainSplineTrajectoryGenerator;
import org.wpilib.math.trajectory.TrajectoryConfig;

class TwoDeadWheelOdometryTest {
  private static final double m_xWheelYPos = 1;
  private static final double m_yWheelXPos = 1;

  private final TwoDeadWheelOdometry m_odometry =
      new TwoDeadWheelOdometry(m_xWheelYPos, m_yWheelXPos, 0.0, 0.0, Rotation2d.ZERO);

  @Test
  void testMultipleConsecutiveUpdates() {
    m_odometry.resetPosition(1, 1, Rotation2d.ZERO, Pose2d.ZERO);

    m_odometry.update(1, 1, Rotation2d.ZERO);
    var secondPose = m_odometry.update(1, 1, Rotation2d.ZERO);

    assertAll(
        () -> assertEquals(0.0, secondPose.getX(), 0.01),
        () -> assertEquals(0.0, secondPose.getY(), 0.01),
        () -> assertEquals(0.0, secondPose.getRotation().getDegrees(), 0.01));
  }

  @Test
  void testTwoIterations() {
    m_odometry.resetPosition(0.0, 0.0, Rotation2d.ZERO, Pose2d.ZERO);

    m_odometry.update(0.0, 0.0, Rotation2d.ZERO);
    final var pose = m_odometry.update(0.1, 0.0, Rotation2d.ZERO);

    assertAll(
        () -> assertEquals(0.1, pose.getX(), 0.01),
        () -> assertEquals(0.0, pose.getY(), 0.01),
        () -> assertEquals(0.0, pose.getRotation().getDegrees(), 0.01));
  }

  @Test
  void testGyroAngleReset() {
    var gyro = Rotation2d.CCW_PI_2;
    var fieldAngle = Rotation2d.ZERO;
    m_odometry.resetPosition(0.0, 0.0, gyro, new Pose2d(Translation2d.ZERO, fieldAngle));
    m_odometry.update(1.0, 0.0, gyro);
    var pose = m_odometry.update(1.0, 0.0, gyro);

    assertAll(
        () -> assertEquals(1.0, pose.getX(), 0.1),
        () -> assertEquals(0.0, pose.getY(), 0.1),
        () -> assertEquals(0.0, pose.getRotation().getRadians(), 0.1));
  }

  @Test
  void testStraightForwardsForwardKinematics() {
    var chassisVelocities = m_odometry.toChassisVelocities(5, 0, 0);

    assertAll(
        () -> assertEquals(5.0, chassisVelocities.vx, 0.1),
        () -> assertEquals(0.0, chassisVelocities.vy, 0.1),
        () -> assertEquals(0.0, chassisVelocities.omega, 0.1));
  }

  @Test
  void testStraightLeftForwardKinematics() {
    var chassisVelocities = m_odometry.toChassisVelocities(0, 5, 0);

    assertAll(
        () -> assertEquals(0.0, chassisVelocities.vx, 0.1),
        () -> assertEquals(5.0, chassisVelocities.vy, 0.1),
        () -> assertEquals(0.0, chassisVelocities.omega, 0.1));
  }

  @Test
  void testSpinInPlaceForwardKinematics() {
    var chassisVelocities = m_odometry.toChassisVelocities(-5, 5, 5);

    assertAll(
        () -> assertEquals(0.0, chassisVelocities.vx, 0.1),
        () -> assertEquals(0.0, chassisVelocities.vy, 0.1),
        () -> assertEquals(5.0, chassisVelocities.omega, 0.1));
  }

  @Test
  void testMixedMotionForwardKinematics() {
    var chassisVelocities = m_odometry.toChassisVelocities(1, -1, 5);

    assertAll(
        () -> assertEquals(6.0, chassisVelocities.vx, 0.1),
        () -> assertEquals(-6.0, chassisVelocities.vy, 0.1),
        () -> assertEquals(5.0, chassisVelocities.omega, 0.1));
  }

  private final SimpleMatrix m_inverseKinematicsMatrix =
      new SimpleMatrix(
          new double[][] {
            {1, 0, -m_xWheelYPos},
            {0, 1, m_yWheelXPos}
          });

  @Test
  void testAccuracyFacingTrajectory() {
    var xWheelPos = 0.0;
    var yWheelPos = 0.0;

    var trajectory =
        DrivetrainSplineTrajectoryGenerator.generate(
            List.of(
                Pose2d.ZERO,
                new Pose2d(20, 20, Rotation2d.fromDegrees(45)),
                new Pose2d(10, 10, Rotation2d.fromDegrees(-90)),
                new Pose2d(30, 30, Rotation2d.fromDegrees(135)),
                new Pose2d(20, 20, Rotation2d.fromDegrees(-90)),
                new Pose2d(10, 10, Rotation2d.ZERO)),
            new TrajectoryConfig(0.5, 2));

    var odometry =
        new TwoDeadWheelOdometry(
            m_xWheelYPos,
            m_yWheelXPos,
            xWheelPos,
            yWheelPos,
            trajectory.start().pose.getRotation(),
            trajectory.start().pose);

    var rand = new Random(5190);

    final double dt = 0.02;
    double t = 0.0;

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    double odometryDistanceTravelled = 0;
    double trajectoryDistanceTravelled = 0;
    while (t <= trajectory.duration) {
      var groundTruthState = trajectory.sampleAt(t);

      trajectoryDistanceTravelled +=
          groundTruthState.forwardVelocity() * dt
              + 0.5 * groundTruthState.forwardAcceleration() * dt * dt;

      var wheelVelocities =
          m_inverseKinematicsMatrix.mult(
              new SimpleMatrix(
                  new double[][] {
                    {groundTruthState.forwardVelocity()},
                    {0},
                    {groundTruthState.forwardVelocity() * groundTruthState.curvature}
                  }));

      var xWheelVel = wheelVelocities.get(0, 0) + rand.nextGaussian() * 0.05;
      var yWheelVel = wheelVelocities.get(1, 0) + rand.nextGaussian() * 0.05;

      xWheelPos += xWheelVel * dt;
      yWheelPos += yWheelVel * dt;

      var lastPose = odometry.getPose();

      // Due to the forward kinematics having a dependency on the gyro angle being accurate, if
      // the gyro angle is noisy, the error *very* quickly compounds.
      // The simulated sensor noise in this class's tests are an order of magnitude lower than the
      // other odometry tests for this reason.
      var xHat =
          odometry.update(
              xWheelPos,
              yWheelPos,
              groundTruthState
                  .pose
                  .getRotation()
                  .plus(new Rotation2d(rand.nextGaussian() * 0.001)));

      odometryDistanceTravelled += lastPose.getTranslation().getDistance(xHat.getTranslation());

      double error = groundTruthState.pose.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(0.0, errorSum / (trajectory.duration / dt), 0.35, "Incorrect mean error");
    assertEquals(0.0, maxError, 0.35, "Incorrect max error");
    assertEquals(
        trajectoryDistanceTravelled,
        odometryDistanceTravelled,
        trajectoryDistanceTravelled * 0.05,
        "Incorrect distance travelled");
  }

  @Test
  void testAccuracyFacingXAxis() {
    var xWheelPos = 0.0;
    var yWheelPos = 0.0;

    var trajectory =
        DrivetrainSplineTrajectoryGenerator.generate(
            List.of(
                Pose2d.ZERO,
                new Pose2d(20, 20, Rotation2d.fromDegrees(45)),
                new Pose2d(10, 10, Rotation2d.fromDegrees(-90)),
                new Pose2d(30, 30, Rotation2d.fromDegrees(135)),
                new Pose2d(20, 20, Rotation2d.fromDegrees(-90)),
                new Pose2d(10, 10, Rotation2d.ZERO)),
            new TrajectoryConfig(0.5, 2));

    var odometry =
        new TwoDeadWheelOdometry(
            m_xWheelYPos, m_yWheelXPos, xWheelPos, yWheelPos, Rotation2d.ZERO, Pose2d.ZERO);

    var rand = new Random(5190);

    final double dt = 0.02;
    double t = 0.0;

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    double odometryDistanceTravelled = 0;
    double trajectoryDistanceTravelled = 0;
    while (t <= trajectory.duration) {
      var groundTruthState = trajectory.sampleAt(t);

      trajectoryDistanceTravelled +=
          groundTruthState.forwardVelocity() * dt
              + 0.5 * groundTruthState.forwardAcceleration() * dt * dt;

      var wheelVelocities =
          m_inverseKinematicsMatrix.mult(
              new SimpleMatrix(
                  new double[][] {
                    {
                      groundTruthState.forwardVelocity()
                          * groundTruthState.pose.getRotation().getCos()
                    },
                    {
                      groundTruthState.forwardVelocity()
                          * groundTruthState.pose.getRotation().getSin()
                    },
                    {0}
                  }));

      var xWheelVel = wheelVelocities.get(0, 0) + rand.nextGaussian() * 0.05;
      var yWheelVel = wheelVelocities.get(1, 0) + rand.nextGaussian() * 0.05;

      xWheelPos += xWheelVel * dt;
      yWheelPos += yWheelVel * dt;

      var lastPose = odometry.getPose();

      // Due to the forward kinematics having a dependency on the gyro angle being accurate, if
      // the gyro angle is noisy, the error *very* quickly compounds.
      // The simulated sensor noise in this class's tests are an order of magnitude lower than the
      // other odometry tests for this reason.
      var xHat = odometry.update(xWheelPos, yWheelPos, new Rotation2d(rand.nextGaussian() * 0.001));

      odometryDistanceTravelled += lastPose.getTranslation().getDistance(xHat.getTranslation());

      double error = groundTruthState.pose.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(0.0, errorSum / (trajectory.duration / dt), 0.15, "Incorrect mean error");
    assertEquals(0.0, maxError, 0.3, "Incorrect max error");
    assertEquals(
        trajectoryDistanceTravelled,
        odometryDistanceTravelled,
        trajectoryDistanceTravelled * 0.05,
        "Incorrect distance travelled");
  }
}
