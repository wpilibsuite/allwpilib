// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisSpeeds;

class TrajectorySampleTest {
  private static final double EPSILON = 1e-9;

  @Test
  void testKinematicInterpolateAtStart() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisSpeeds(1, 0, 0), new ChassisAccelerations());

    var end =
        new TrajectorySample(
            1,
            new Pose2d(1, 0, Rotation2d.kZero),
            new ChassisSpeeds(2, 0, 0),
            new ChassisAccelerations());

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0);

    assertEquals(start.timestamp, interpolated.timestamp);
    assertEquals(start.pose, interpolated.pose);
    assertEquals(start.velocity, interpolated.velocity);
    assertEquals(start.acceleration, interpolated.acceleration);
  }

  @Test
  void testKinematicInterpolateAtEnd() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisSpeeds(1, 0, 0), new ChassisAccelerations());

    var end =
        new TrajectorySample(
            1,
            new Pose2d(1, 0, Rotation2d.kZero),
            new ChassisSpeeds(2, 0, 0),
            new ChassisAccelerations());

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 1);

    assertEquals(end.timestamp, interpolated.timestamp);
    assertEquals(end.pose, interpolated.pose);
    assertEquals(end.velocity, interpolated.velocity);
    assertEquals(end.acceleration, interpolated.acceleration);
  }

  @Test
  void testKinematicInterpolateMidpoint() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisSpeeds(1, 0, 0), new ChassisAccelerations(0.2, 0.0, 0.0));

    var end =
        new TrajectorySample(
            2,
            new Pose2d(2, 0, Rotation2d.kZero),
            new ChassisSpeeds(2, 0, 0),
            new ChassisAccelerations(0.6, 0.0, 0.0));

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0.5);

    // Expected timestamp at midpoint
    double expectedTimestamp = 0 + (2 - 0) * 0.5;
    assertEquals(expectedTimestamp, interpolated.timestamp, EPSILON);

    // Using constant-acceleration kinematic equations with start's accelerations
    // deltaT = expectedTimestamp - start.timestamp = 1.0
    double deltaT = expectedTimestamp - start.timestamp;

    // vₖ₊₁ = vₖ + aₖΔt
    double expectedVx = start.velocity.vx + start.acceleration.ax * deltaT;
    double expectedVy = start.velocity.vy + start.acceleration.ay * deltaT;

    // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)²
    double expectedX =
        start.pose.getX()
            + start.velocity.vx * deltaT
            + 0.5 * start.acceleration.ax * deltaT * deltaT;
    double expectedY =
        start.pose.getY()
            + start.velocity.vy * deltaT
            + 0.5 * start.acceleration.ay * deltaT * deltaT;

    // Linear interpolation of accelerations
    double expectedAx = start.acceleration.ax + (end.acceleration.ax - start.acceleration.ax) * 0.5;
    double expectedAy = start.acceleration.ay + (end.acceleration.ay - start.acceleration.ay) * 0.5;

    assertEquals(expectedX, interpolated.pose.getX(), EPSILON);
    assertEquals(expectedY, interpolated.pose.getY(), EPSILON);
    assertEquals(expectedVx, interpolated.velocity.vx, EPSILON);
    assertEquals(expectedVy, interpolated.velocity.vy, EPSILON);
    assertEquals(expectedAx, interpolated.acceleration.ax, EPSILON);
    assertEquals(expectedAy, interpolated.acceleration.ay, EPSILON);
  }

  @Test
  void testKinematicInterpolateWithAcceleration() {
    var start =
        new TrajectorySample(
            0,
            Pose2d.kZero,
            new ChassisSpeeds(0, 0, 0),
            new ChassisAccelerations(1.0, 0, 0)); // 1 m/s²

    var end =
        new TrajectorySample(
            1,
            new Pose2d(0.5, 0, Rotation2d.kZero),
            new ChassisSpeeds(1, 0, 0),
            new ChassisAccelerations());

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0.5);

    // Expected timestamp
    double expectedTimestamp = 0 + (1 - 0) * 0.5;
    assertEquals(expectedTimestamp, interpolated.timestamp, EPSILON);

    // Constant-acceleration kinematics from start with deltaT = 0.5
    double deltaT = expectedTimestamp - start.timestamp;

    // vₖ₊₁ = vₖ + aₖΔt = 0 + 1.0*0.5 = 0.5
    double expectedVx = start.velocity.vx + start.acceleration.ax * deltaT;

    // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)² = 0 + 0*0.5 + ½*1.0*0.25 = 0.125
    double expectedX =
        start.pose.getX()
            + start.velocity.vx * deltaT
            + 0.5 * start.acceleration.ax * deltaT * deltaT;

    // Linear interpolation of acceleration at t=0.5: ax = 1.0 + (0 - 1.0)*0.5 = 0.5
    double expectedAx = start.acceleration.ax + (end.acceleration.ax - start.acceleration.ax) * 0.5;

    assertEquals(expectedX, interpolated.pose.getX(), EPSILON);
    assertEquals(expectedVx, interpolated.velocity.vx, EPSILON);
    assertEquals(expectedAx, interpolated.acceleration.ax, EPSILON);
  }

  @Test
  void testKinematicInterpolateAngularVelocity() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisSpeeds(1, 0, 0), new ChassisAccelerations(0.0, 0.0, 0.1));

    var end =
        new TrajectorySample(
            1,
            new Pose2d(1, 0, Rotation2d.fromDegrees(90)),
            new ChassisSpeeds(1, 0, Math.PI / 2),
            new ChassisAccelerations(0.0, 0.0, 0.5));

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0.5);

    // Expected timestamp
    double expectedTimestamp = 0 + (1 - 0) * 0.5;
    assertEquals(expectedTimestamp, interpolated.timestamp, EPSILON);

    double deltaT = expectedTimestamp - start.timestamp;

    // vₖ₊₁ = vₖ + aₖΔt (with zero acceleration)
    double expectedOmega = start.velocity.omega + start.acceleration.alpha * deltaT;

    // Linear interpolation of angular acceleration
    double expectedAlpha =
        start.acceleration.alpha + (end.acceleration.alpha - start.acceleration.alpha) * 0.5;

    // θₖ₊₁ = θₖ + ωₖΔt + ½α(Δt)²
    double expectedTheta =
        start.pose.getRotation().getRadians()
            + start.velocity.omega * deltaT
            + 0.5 * start.acceleration.alpha * deltaT * deltaT;

    assertEquals(expectedOmega, interpolated.velocity.omega, EPSILON);
    assertEquals(expectedAlpha, interpolated.acceleration.alpha, EPSILON);
    assertEquals(expectedTheta, interpolated.pose.getRotation().getRadians(), EPSILON);
  }

  @Test
  void testKinematicInterpolateMultipleQuarters() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisSpeeds(1, 0, 0), new ChassisAccelerations(0.2, 0.0, 0.0));

    var end =
        new TrajectorySample(
            2,
            new Pose2d(2, 0, Rotation2d.kZero),
            new ChassisSpeeds(2, 0, 0),
            new ChassisAccelerations(0.4, 0.0, 0.0));

    // Test at 0.25
    var q1 = TrajectorySample.kinematicInterpolate(start, end, 0.25);
    double expectedT1 = 0 + (2 - 0) * 0.25;
    double deltaT1 = expectedT1 - start.timestamp;
    double expectedX1 =
        start.pose.getX()
            + start.velocity.vx * deltaT1
            + 0.5 * start.acceleration.ax * deltaT1 * deltaT1;
    double expectedVx1 = start.velocity.vx + start.acceleration.ax * deltaT1;

    assertEquals(expectedT1, q1.timestamp, EPSILON);
    assertEquals(expectedX1, q1.pose.getX(), EPSILON);
    assertEquals(expectedVx1, q1.velocity.vx, EPSILON);

    // Test at 0.75
    var q3 = TrajectorySample.kinematicInterpolate(start, end, 0.75);
    double expectedT3 = 0 + (2 - 0) * 0.75;
    double deltaT3 = expectedT3 - start.timestamp;
    double expectedX3 =
        start.pose.getX()
            + start.velocity.vx * deltaT3
            + 0.5 * start.acceleration.ax * deltaT3 * deltaT3;
    double expectedVx3 = start.velocity.vx + start.acceleration.ax * deltaT3;

    assertEquals(expectedT3, q3.timestamp, EPSILON);
    assertEquals(expectedX3, q3.pose.getX(), EPSILON);
    assertEquals(expectedVx3, q3.velocity.vx, EPSILON);
  }

  @Test
  void testKinematicInterpolateNegativeVelocity() {
    var start =
        new TrajectorySample(
            0,
            new Pose2d(1, 0, Rotation2d.kZero),
            new ChassisSpeeds(-1, 0, 0),
            new ChassisAccelerations(-0.5, 0.0, 0.0));

    var end =
        new TrajectorySample(
            1,
            new Pose2d(0, 0, Rotation2d.kZero),
            new ChassisSpeeds(-2, 0, 0),
            new ChassisAccelerations());

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0.5);

    // Expected timestamp
    double expectedTimestamp = 0 + (1 - 0) * 0.5;
    assertEquals(expectedTimestamp, interpolated.timestamp, EPSILON);

    // Constant-acceleration kinematics from start
    double deltaT = expectedTimestamp - start.timestamp;

    // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)² = 1 + (-1)*0.5 + 0 = 0.5
    double expectedX =
        start.pose.getX()
            + start.velocity.vx * deltaT
            + 0.5 * start.acceleration.ax * deltaT * deltaT;

    assertEquals(expectedX, interpolated.pose.getX(), EPSILON);
  }

  @Test
  void testKinematicInterpolateLateral() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisSpeeds(0, 1, 0), new ChassisAccelerations(0.0, 0.2, 0.0));

    var end =
        new TrajectorySample(
            1,
            new Pose2d(0, 1, Rotation2d.kZero),
            new ChassisSpeeds(0, 2, 0),
            new ChassisAccelerations(0.0, 0.6, 0.0));

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0.5);

    // Expected timestamp
    double expectedTimestamp = 0 + (1 - 0) * 0.5;
    assertEquals(expectedTimestamp, interpolated.timestamp, EPSILON);

    // Constant-acceleration kinematics from start with deltaT = 0.5
    double deltaT = expectedTimestamp - start.timestamp;

    // yₖ₊₁ = yₖ + vₖΔt + ½a(Δt)² = 0 + 1*0.5 + 0 = 0.5
    double expectedY =
        start.pose.getY()
            + start.velocity.vy * deltaT
            + 0.5 * start.acceleration.ay * deltaT * deltaT;

    // vₖ₊₁ = vₖ + aₖΔt = 1 + 0*0.5 = 1.0
    double expectedVy = start.velocity.vy + start.acceleration.ay * deltaT;

    // Linear interpolation of acceleration
    double expectedAy = start.acceleration.ay + (end.acceleration.ay - start.acceleration.ay) * 0.5;

    assertEquals(0.0, interpolated.pose.getX(), EPSILON);
    assertEquals(expectedY, interpolated.pose.getY(), EPSILON);
    assertEquals(expectedVy, interpolated.velocity.vy, EPSILON);
    assertEquals(expectedAy, interpolated.acceleration.ay, EPSILON);
  }

  @Test
  void testKinematicInterpolateMonotonicity() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisSpeeds(1, 0, 0), new ChassisAccelerations());

    var end =
        new TrajectorySample(
            1,
            new Pose2d(1, 0, Rotation2d.kZero),
            new ChassisSpeeds(2, 0, 0),
            new ChassisAccelerations());

    var prev = start;
    for (double t = 0.1; t <= 1.0; t += 0.1) {
      var curr = TrajectorySample.kinematicInterpolate(start, end, t);
      // X position should be monotonically increasing
      assertTrue(curr.pose.getX() >= prev.pose.getX() - EPSILON);
      // Timestamp should be monotonically increasing
      assertTrue(curr.timestamp >= prev.timestamp - EPSILON);
      prev = curr;
    }
  }

  @Test
  void testKinematicInterpolateContinuity() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisSpeeds(1, 0, 0), new ChassisAccelerations());

    var end =
        new TrajectorySample(
            1,
            new Pose2d(1, 0, Rotation2d.kZero),
            new ChassisSpeeds(2, 0, 0),
            new ChassisAccelerations());

    // Interpolate at close points
    var t1 = TrajectorySample.kinematicInterpolate(start, end, 0.5);
    var t2 = TrajectorySample.kinematicInterpolate(start, end, 0.500001);

    // Positions should be very close
    var dx = Math.abs(t2.pose.getX() - t1.pose.getX());
    var dy = Math.abs(t2.pose.getY() - t1.pose.getY());
    assertTrue(dx < 1e-4);
    assertTrue(dy < 1e-4);
  }

  @Test
  void testKinematicInterpolateZeroTime() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisSpeeds(1, 0, 0), new ChassisAccelerations());

    var end =
        new TrajectorySample(
            0,
            new Pose2d(1, 0, Rotation2d.kZero),
            new ChassisSpeeds(2, 0, 0),
            new ChassisAccelerations());

    // Should handle zero time difference gracefully
    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0.5);
    assertEquals(0.0, interpolated.timestamp, EPSILON);
  }
}
