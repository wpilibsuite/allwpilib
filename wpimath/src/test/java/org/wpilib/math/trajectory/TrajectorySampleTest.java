// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.util.MathUtil;

class TrajectorySampleTest {
  private static final double EPSILON = 1e-9;

  @Test
  void testKinematicInterpolateAtStart() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisVelocities(1, 0, 0), new ChassisAccelerations());

    var end =
        new TrajectorySample(
            1,
            new Pose2d(1, 0, Rotation2d.kZero),
            new ChassisVelocities(2, 0, 0),
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
            0, Pose2d.kZero, new ChassisVelocities(1, 0, 0), new ChassisAccelerations());

    var end =
        new TrajectorySample(
            1,
            new Pose2d(1, 0, Rotation2d.kZero),
            new ChassisVelocities(2, 0, 0),
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
            0,
            Pose2d.kZero,
            new ChassisVelocities(1, 0, 0),
            new ChassisAccelerations(0.2, 0.0, 0.0));

    var end =
        new TrajectorySample(
            2,
            new Pose2d(2, 0, Rotation2d.kZero),
            new ChassisVelocities(2, 0, 0),
            new ChassisAccelerations(0.6, 0.0, 0.0));

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0.5);

    // Expected timestamp at midpoint
    double expectedTimestamp = 0 + (2 - 0) * 0.5;
    assertEquals(expectedTimestamp, interpolated.timestamp, EPSILON);

    // Using constant-acceleration kinematic equations with start's accelerations
    // deltaT = expectedTimestamp - start.timestamp = 1.0
    double deltaT = expectedTimestamp - start.timestamp;

    // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)²
    double expectedX =
        start.pose.getX()
            + start.velocity.vx * deltaT
            + 0.5 * start.acceleration.ax * deltaT * deltaT;
    double expectedY =
        start.pose.getY()
            + start.velocity.vy * deltaT
            + 0.5 * start.acceleration.ay * deltaT * deltaT;

    // vₖ₊₁ = vₖ + aₖΔt
    final double expectedVx = start.velocity.vx + start.acceleration.ax * deltaT;
    final double expectedVy = start.velocity.vy + start.acceleration.ay * deltaT;

    // Constant acceleration
    final double expectedAx = start.acceleration.ax;
    final double expectedAy = start.acceleration.ay;

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
            new ChassisVelocities(0, 0, 0),
            new ChassisAccelerations(1.0, 0, 0)); // 1 m/s²

    var end =
        new TrajectorySample(
            1,
            new Pose2d(0.5, 0, Rotation2d.kZero),
            new ChassisVelocities(1, 0, 0),
            new ChassisAccelerations());

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0.5);

    // Expected timestamp
    double expectedTimestamp = 0 + (1 - 0) * 0.5;
    assertEquals(expectedTimestamp, interpolated.timestamp, EPSILON);

    // Constant-acceleration kinematics from start with deltaT = 0.5
    double deltaT = expectedTimestamp - start.timestamp;

    // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)² = 0 + 0*0.5 + ½*1.0*0.25 = 0.125
    double expectedX =
        start.pose.getX()
            + start.velocity.vx * deltaT
            + 0.5 * start.acceleration.ax * deltaT * deltaT;

    // vₖ₊₁ = vₖ + aₖΔt = 0 + 1.0*0.5 = 0.5
    double expectedVx = start.velocity.vx + start.acceleration.ax * deltaT;

    // Constant acceleration
    double expectedAx = start.acceleration.ax;

    assertEquals(expectedX, interpolated.pose.getX(), EPSILON);
    assertEquals(expectedVx, interpolated.velocity.vx, EPSILON);
    assertEquals(expectedAx, interpolated.acceleration.ax, EPSILON);
  }

  @Test
  void testKinematicInterpolateAngularVelocity() {
    var start =
        new TrajectorySample(
            0,
            Pose2d.kZero,
            new ChassisVelocities(1, 0, 0),
            new ChassisAccelerations(0.0, 0.0, 0.1));

    var end =
        new TrajectorySample(
            1,
            new Pose2d(1, 0, Rotation2d.fromDegrees(90)),
            new ChassisVelocities(1, 0, Math.PI / 2),
            new ChassisAccelerations(0.0, 0.0, 0.5));

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0.5);

    // Expected timestamp
    double expectedTimestamp = 0 + (1 - 0) * 0.5;
    assertEquals(expectedTimestamp, interpolated.timestamp, EPSILON);

    double deltaT = expectedTimestamp - start.timestamp;

    // θₖ₊₁ = θₖ + ωₖΔt + ½α(Δt)²
    double expectedTheta =
        start.pose.getRotation().getRadians()
            + start.velocity.omega * deltaT
            + 0.5 * start.acceleration.alpha * deltaT * deltaT;

    // vₖ₊₁ = vₖ + aₖΔt (with zero acceleration)
    double expectedOmega = start.velocity.omega + start.acceleration.alpha * deltaT;

    // Constant angular acceleration
    double expectedAlpha = start.acceleration.alpha;

    assertEquals(expectedOmega, interpolated.velocity.omega, EPSILON);
    assertEquals(expectedAlpha, interpolated.acceleration.alpha, EPSILON);
    assertEquals(expectedTheta, interpolated.pose.getRotation().getRadians(), EPSILON);
  }

  @Test
  void testKinematicInterpolateMultipleQuarters() {
    var start =
        new TrajectorySample(
            0,
            Pose2d.kZero,
            new ChassisVelocities(1, 0, 0),
            new ChassisAccelerations(0.2, 0.0, 0.0));

    var end =
        new TrajectorySample(
            2,
            new Pose2d(2, 0, Rotation2d.kZero),
            new ChassisVelocities(2, 0, 0),
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
            new ChassisVelocities(-1, 0, 0),
            new ChassisAccelerations(-0.5, 0.0, 0.0));

    var end =
        new TrajectorySample(
            1,
            new Pose2d(0, 0, Rotation2d.kZero),
            new ChassisVelocities(-2, 0, 0),
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
            0,
            Pose2d.kZero,
            new ChassisVelocities(0, 1, 0),
            new ChassisAccelerations(0.0, 0.2, 0.0));

    var end =
        new TrajectorySample(
            1,
            new Pose2d(0, 1, Rotation2d.kZero),
            new ChassisVelocities(0, 2, 0),
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
    final double expectedVy = start.velocity.vy + start.acceleration.ay * deltaT;

    // Constant acceleration
    final double expectedAy = start.acceleration.ay;

    assertEquals(0.0, interpolated.pose.getX(), EPSILON);
    assertEquals(expectedY, interpolated.pose.getY(), EPSILON);
    assertEquals(expectedVy, interpolated.velocity.vy, EPSILON);
    assertEquals(expectedAy, interpolated.acceleration.ay, EPSILON);
  }

  @Test
  void testKinematicInterpolateMonotonicity() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisVelocities(1, 0, 0), new ChassisAccelerations());

    var end =
        new TrajectorySample(
            1,
            new Pose2d(1, 0, Rotation2d.kZero),
            new ChassisVelocities(2, 0, 0),
            new ChassisAccelerations());

    var prev = start;
    for (int t = 1; t <= 10; t++) {
      var curr = TrajectorySample.kinematicInterpolate(start, end, t / 10.0);
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
            0, Pose2d.kZero, new ChassisVelocities(1, 0, 0), new ChassisAccelerations());

    var end =
        new TrajectorySample(
            1,
            new Pose2d(1, 0, Rotation2d.kZero),
            new ChassisVelocities(2, 0, 0),
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
  void testKinematicInterpolateNonzeroStartTimestamp() {
    // Regression test: integration must use the elapsed time from the start
    // sample (deltaT), not the absolute interpolated timestamp. This only
    // differs when the start sample's timestamp is nonzero.
    var start =
        new TrajectorySample(
            10,
            Pose2d.kZero,
            new ChassisVelocities(1, 0, 0),
            new ChassisAccelerations(2.0, 0.0, 0.0));

    var end =
        new TrajectorySample(
            12,
            new Pose2d(8, 0, Rotation2d.kZero),
            new ChassisVelocities(5, 0, 0),
            new ChassisAccelerations(2.0, 0.0, 0.0));

    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0.5);

    // Absolute interpolated timestamp
    double expectedTimestamp = MathUtil.lerp(start.timestamp, end.timestamp, 0.5);
    assertEquals(11.0, expectedTimestamp, EPSILON);
    assertEquals(expectedTimestamp, interpolated.timestamp, EPSILON);

    // Elapsed time from start sample
    double deltaT = expectedTimestamp - start.timestamp;
    assertEquals(1.0, deltaT, EPSILON);

    // vₖ₊₁ = vₖ + aₖΔt = 1 + 2*1 = 3 (would be 1 + 2*11 = 23 with the bug)
    double expectedVx = start.velocity.vx + start.acceleration.ax * deltaT;
    assertEquals(3.0, expectedVx, EPSILON);
    assertEquals(expectedVx, interpolated.velocity.vx, EPSILON);

    // xₖ₊₁ = xₖ + vₖΔt + ½aₖ(Δt)² = 0 + 1*1 + ½*2*1 = 2
    double expectedX =
        start.pose.getX()
            + start.velocity.vx * deltaT
            + 0.5 * start.acceleration.ax * deltaT * deltaT;
    assertEquals(2.0, expectedX, EPSILON);
    assertEquals(expectedX, interpolated.pose.getX(), EPSILON);
  }

  @Test
  void testKinematicInterpolateZeroTime() {
    var start =
        new TrajectorySample(
            0, Pose2d.kZero, new ChassisVelocities(1, 0, 0), new ChassisAccelerations());

    var end =
        new TrajectorySample(
            0,
            new Pose2d(1, 0, Rotation2d.kZero),
            new ChassisVelocities(2, 0, 0),
            new ChassisAccelerations());

    // Should handle zero time difference gracefully
    var interpolated = TrajectorySample.kinematicInterpolate(start, end, 0.5);
    assertEquals(0.0, interpolated.timestamp, EPSILON);
  }

  @Test
  void testSplineSampleStoresFieldRelativeVelocity() {
    // A SplineSample is built from path-relative (forward) scalars but stores
    // velocity/acceleration in the field frame. For a robot facing +90 degrees
    // moving forward, the field velocity should point along +y.
    double forwardVelocity = 2.0;
    double forwardAcceleration = 1.5;
    double curvature = 0.25;
    var sample =
        new SplineSample(
            0.0,
            new Pose2d(0, 0, Rotation2d.kCCW_Pi_2),
            forwardVelocity,
            forwardAcceleration,
            curvature);

    // Field-relative: forward speed rotated into +y.
    assertEquals(0.0, sample.velocity.vx, EPSILON);
    assertEquals(forwardVelocity, sample.velocity.vy, EPSILON);
    // Omega is frame-invariant and equals forward * curvature.
    assertEquals(forwardVelocity * curvature, sample.velocity.omega, EPSILON);

    assertEquals(0.0, sample.acceleration.ax, EPSILON);
    assertEquals(forwardAcceleration, sample.acceleration.ay, EPSILON);

    // The projection accessors recover the path-relative scalars.
    assertEquals(forwardVelocity, sample.forwardVelocity(), EPSILON);
    assertEquals(forwardAcceleration, sample.forwardAcceleration(), EPSILON);
  }

  @Test
  void testTransformRotatesVelocityAndAcceleration() {
    // Field-relative velocity/acceleration must rotate with the transform's rotation.
    var sample =
        new TrajectorySample(
            0,
            new Pose2d(0, 0, Rotation2d.kZero),
            new ChassisVelocities(1, 0, 0.5),
            new ChassisAccelerations(2, 0, 0.3));

    var transformed =
        sample.transform(new Transform2d(new Translation2d(3, 4), Rotation2d.kCCW_Pi_2));

    // Pose is transformed.
    assertEquals(3.0, transformed.pose.getX(), EPSILON);
    assertEquals(4.0, transformed.pose.getY(), EPSILON);
    assertEquals(Math.PI / 2, transformed.pose.getRotation().getRadians(), EPSILON);

    // Velocity/acceleration vectors rotate by +90 degrees; angular terms are unchanged.
    assertEquals(0.0, transformed.velocity.vx, EPSILON);
    assertEquals(1.0, transformed.velocity.vy, EPSILON);
    assertEquals(0.5, transformed.velocity.omega, EPSILON);
    assertEquals(0.0, transformed.acceleration.ax, EPSILON);
    assertEquals(2.0, transformed.acceleration.ay, EPSILON);
    assertEquals(0.3, transformed.acceleration.alpha, EPSILON);
  }

  @Test
  void testRelativeToRotatesVelocityAndAcceleration() {
    // relativeTo re-expresses the sample in a frame rotated by the other pose's
    // rotation, so field-relative velocity/acceleration rotate by the negative of it.
    var sample =
        new TrajectorySample(
            0,
            new Pose2d(1, 2, Rotation2d.kCCW_Pi_2),
            new ChassisVelocities(0, 1, 0.5),
            new ChassisAccelerations(0, 2, 0.3));

    var relative = sample.relativeTo(new Pose2d(1, 2, Rotation2d.kCCW_Pi_2));

    // Pose becomes the origin.
    assertEquals(0.0, relative.pose.getX(), EPSILON);
    assertEquals(0.0, relative.pose.getY(), EPSILON);
    assertEquals(0.0, relative.pose.getRotation().getRadians(), EPSILON);

    // Velocity/acceleration vectors rotate by -90 degrees; angular terms are unchanged.
    assertEquals(1.0, relative.velocity.vx, EPSILON);
    assertEquals(0.0, relative.velocity.vy, EPSILON);
    assertEquals(0.5, relative.velocity.omega, EPSILON);
    assertEquals(2.0, relative.acceleration.ax, EPSILON);
    assertEquals(0.0, relative.acceleration.ay, EPSILON);
    assertEquals(0.3, relative.acceleration.alpha, EPSILON);
  }

  @Test
  void testSplineSampleTransformPreservesForwardScalars() {
    // Rotating the sample rotates both the heading and the field velocity, so the
    // heading-relative forward scalars (and curvature) are invariant.
    var sample =
        new SplineSample(0.0, new Pose2d(1, 2, Rotation2d.fromDegrees(20)), 2.0, 1.5, 0.25);

    var transformed =
        sample.transform(new Transform2d(new Translation2d(3, 4), Rotation2d.fromDegrees(35)));
    var relative = sample.relativeTo(new Pose2d(0, 0, Rotation2d.fromDegrees(-15)));

    for (var s : new SplineSample[] {transformed, relative}) {
      assertEquals(2.0, s.forwardVelocity(), EPSILON);
      assertEquals(1.5, s.forwardAcceleration(), EPSILON);
      assertEquals(0.25, s.curvature, EPSILON);
    }
  }

  @Test
  void testDifferentialSampleTransformPreservesWheelSpeeds() {
    // Wheel speeds are frame-invariant, so they survive a transform unchanged while
    // the field-relative velocity rotates.
    var kinematics = new DifferentialDriveKinematics(0.5);
    var sample =
        new DifferentialSample(
            0.0,
            new Pose2d(0, 0, Rotation2d.kZero),
            new ChassisVelocities(2, 0, 0.5),
            new ChassisAccelerations(1, 0, 0.2),
            kinematics);

    var transformed =
        sample.transform(new Transform2d(new Translation2d(0, 0), Rotation2d.kCCW_Pi_2));

    assertEquals(sample.leftSpeed, transformed.leftSpeed, EPSILON);
    assertEquals(sample.rightSpeed, transformed.rightSpeed, EPSILON);

    // The field velocity rotates by +90 degrees.
    assertEquals(0.0, transformed.velocity.vx, EPSILON);
    assertEquals(2.0, transformed.velocity.vy, EPSILON);
    assertEquals(0.5, transformed.velocity.omega, EPSILON);
  }
}
