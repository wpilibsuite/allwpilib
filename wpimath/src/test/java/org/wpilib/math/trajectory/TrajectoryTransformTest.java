// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;

class TrajectoryTransformTest {
  private static HolonomicTrajectory makeHolonomicTrajectory() {
    return new HolonomicTrajectory(
        List.of(
            new HolonomicSample(
                0,
                new Pose2d(1, 2, Rotation2d.fromDegrees(30)),
                new ChassisVelocities(1, 0.5, 0.2),
                new ChassisAccelerations(0.3, -0.1, 0.05)),
            new HolonomicSample(
                1,
                new Pose2d(2, 2.5, Rotation2d.fromDegrees(40)),
                new ChassisVelocities(1.5, -0.25, 0.1),
                new ChassisAccelerations(-0.2, 0.4, -0.1)),
            new HolonomicSample(
                2,
                new Pose2d(2.5, 3.5, Rotation2d.fromDegrees(60)),
                new ChassisVelocities(0.5, 1, -0.3),
                new ChassisAccelerations(0.1, 0.2, 0.15))));
  }

  @Test
  void testHolonomicTransformBy() {
    var trajectory = makeHolonomicTrajectory();
    var transform = new Transform2d(new Translation2d(1, 2), Rotation2d.fromDegrees(30));

    var transformedTrajectory = trajectory.transformBy(transform);

    // Test initial pose
    assertEquals(
        trajectory.start().pose.transformBy(transform), transformedTrajectory.sampleAt(0).pose);

    // Field-relative velocity and acceleration rotate by the transform's rotation
    for (int i = 0; i < trajectory.samples.size(); i++) {
      var sample = trajectory.samples.get(i);
      var transformedSample = transformedTrajectory.samples.get(i);

      var expectedVelocity = sample.velocity.toFieldRelative(transform.getRotation());
      assertEquals(expectedVelocity.vx, transformedSample.velocity.vx, 1e-9);
      assertEquals(expectedVelocity.vy, transformedSample.velocity.vy, 1e-9);
      assertEquals(expectedVelocity.omega, transformedSample.velocity.omega, 1e-9);

      var expectedAcceleration = sample.acceleration.toFieldRelative(transform.getRotation());
      assertEquals(expectedAcceleration.ax, transformedSample.acceleration.ax, 1e-9);
      assertEquals(expectedAcceleration.ay, transformedSample.acceleration.ay, 1e-9);
      assertEquals(expectedAcceleration.alpha, transformedSample.acceleration.alpha, 1e-9);
    }

    testSameShapedHolonomicTrajectory(trajectory.samples, transformedTrajectory.samples);
    testSameRobotRelativeMotion(trajectory.samples, transformedTrajectory.samples);
  }

  @Test
  void testHolonomicRelativeTo() {
    var trajectory = makeHolonomicTrajectory();

    var transformedTrajectory = trajectory.relativeTo(new Pose2d(1, 2, Rotation2d.fromDegrees(30)));

    // Test initial pose
    assertEquals(Pose2d.ZERO, transformedTrajectory.sampleAt(0).pose);

    testSameShapedHolonomicTrajectory(trajectory.samples, transformedTrajectory.samples);
    testSameRobotRelativeMotion(trajectory.samples, transformedTrajectory.samples);
  }

  // Differential drives can't move sideways, so the robot-relative velocity and acceleration have
  // no lateral component
  private static DifferentialSample makeDifferentialSample(
      double time,
      Pose2d pose,
      double v,
      double omega,
      double a,
      double alpha,
      double leftVelocity,
      double rightVelocity) {
    return new DifferentialSample(
        time,
        pose,
        new ChassisVelocities(v, 0, omega).toFieldRelative(pose.getRotation()),
        new ChassisAccelerations(a, 0, alpha).toFieldRelative(pose.getRotation()),
        leftVelocity,
        rightVelocity);
  }

  private static DifferentialTrajectory makeDifferentialTrajectory() {
    return new DifferentialTrajectory(
        List.of(
            makeDifferentialSample(
                0, new Pose2d(1, 2, Rotation2d.fromDegrees(30)), 1, 0.2, 0.3, 0.05, 0.95, 1.05),
            makeDifferentialSample(
                1,
                new Pose2d(2, 2.5, Rotation2d.fromDegrees(40)),
                1.5,
                0.1,
                -0.2,
                -0.1,
                1.475,
                1.525),
            makeDifferentialSample(
                2,
                new Pose2d(2.5, 3.5, Rotation2d.fromDegrees(60)),
                0.5,
                -0.3,
                0.1,
                0.15,
                0.575,
                0.425)));
  }

  @Test
  void testDifferentialTransformBy() {
    var trajectory = makeDifferentialTrajectory();
    var transform = new Transform2d(new Translation2d(1, 2), Rotation2d.fromDegrees(30));

    var transformedTrajectory = trajectory.transformBy(transform);

    // Test initial pose
    assertEquals(
        trajectory.start().pose.transformBy(transform), transformedTrajectory.sampleAt(0).pose);

    // Field-relative velocity and acceleration rotate by the transform's rotation
    for (int i = 0; i < trajectory.samples.size(); i++) {
      var sample = trajectory.samples.get(i);
      var transformedSample = transformedTrajectory.samples.get(i);

      var expectedVelocity = sample.velocity.toFieldRelative(transform.getRotation());
      assertEquals(expectedVelocity.vx, transformedSample.velocity.vx, 1e-9);
      assertEquals(expectedVelocity.vy, transformedSample.velocity.vy, 1e-9);
      assertEquals(expectedVelocity.omega, transformedSample.velocity.omega, 1e-9);

      var expectedAcceleration = sample.acceleration.toFieldRelative(transform.getRotation());
      assertEquals(expectedAcceleration.ax, transformedSample.acceleration.ax, 1e-9);
      assertEquals(expectedAcceleration.ay, transformedSample.acceleration.ay, 1e-9);
      assertEquals(expectedAcceleration.alpha, transformedSample.acceleration.alpha, 1e-9);
    }

    testSameShapedHolonomicTrajectory(trajectory.samples, transformedTrajectory.samples);
    testSameRobotRelativeMotion(trajectory.samples, transformedTrajectory.samples);
    testSameWheelVelocities(trajectory.samples, transformedTrajectory.samples);
  }

  @Test
  void testDifferentialRelativeTo() {
    var trajectory = makeDifferentialTrajectory();

    var transformedTrajectory = trajectory.relativeTo(new Pose2d(1, 2, Rotation2d.fromDegrees(30)));

    // Test initial pose
    assertEquals(Pose2d.ZERO, transformedTrajectory.sampleAt(0).pose);

    testSameShapedHolonomicTrajectory(trajectory.samples, transformedTrajectory.samples);
    testSameRobotRelativeMotion(trajectory.samples, transformedTrajectory.samples);
    testSameWheelVelocities(trajectory.samples, transformedTrajectory.samples);
  }

  @Test
  void testTransformBy() {
    var config = new TrajectoryConfig(3, 3);
    var trajectory =
        DrivetrainSplineTrajectoryGenerator.generate(
            Pose2d.ZERO, List.of(), new Pose2d(1, 1, Rotation2d.CCW_PI_2), config);

    var transformedTrajectory =
        trajectory.transformBy(
            new Transform2d(new Translation2d(1, 2), Rotation2d.fromDegrees(30)));

    // Test initial pose
    assertEquals(
        new Pose2d(1, 2, Rotation2d.fromDegrees(30)), transformedTrajectory.sampleAt(0).pose);

    testSameShapedTrajectory(trajectory.samples, transformedTrajectory.samples);
    testSameForwardScalars(trajectory.samples, transformedTrajectory.samples);
  }

  @Test
  void testRelativeTo() {
    var config = new TrajectoryConfig(3, 3);
    var trajectory =
        DrivetrainSplineTrajectoryGenerator.generate(
            new Pose2d(1, 2, Rotation2d.fromDegrees(30.0)),
            List.of(),
            new Pose2d(5, 7, Rotation2d.CCW_PI_2),
            config);

    var transformedTrajectory = trajectory.relativeTo(new Pose2d(1, 2, Rotation2d.fromDegrees(30)));

    // Test initial pose
    assertEquals(Pose2d.ZERO, transformedTrajectory.sampleAt(0).pose);

    testSameShapedTrajectory(trajectory.samples, transformedTrajectory.samples);
    testSameForwardScalars(trajectory.samples, transformedTrajectory.samples);
  }

  // A rigid transform rotates both the heading and the field-relative velocity/acceleration by the
  // same amount, so the heading-relative forward scalars (and curvature) are invariant. This would
  // fail if transformBy/relativeTo rotated the pose but not the velocity/acceleration.
  void testSameForwardScalars(
      List<DrivetrainSplineSample> statesA, List<DrivetrainSplineSample> statesB) {
    assertEquals(statesA.size(), statesB.size());
    for (int i = 0; i < statesA.size(); i++) {
      assertEquals(statesA.get(i).forwardVelocity(), statesB.get(i).forwardVelocity(), 1e-9);
      assertEquals(
          statesA.get(i).forwardAcceleration(), statesB.get(i).forwardAcceleration(), 1e-9);
      assertEquals(statesA.get(i).curvature, statesB.get(i).curvature, 1e-9);
    }
  }

  void testSameShapedTrajectory(
      List<DrivetrainSplineSample> statesA, List<DrivetrainSplineSample> statesB) {
    for (int i = 0; i < statesA.size() - 1; i++) {
      var a1 = statesA.get(i).pose;
      var a2 = statesA.get(i + 1).pose;

      var b1 = statesB.get(i).pose;
      var b2 = statesB.get(i + 1).pose;

      var expectedRel = a2.relativeTo(a1);
      var actualRel = b2.relativeTo(b1);

      assertEquals(
          expectedRel,
          actualRel,
          String.format(
              "pose mismatch at index %d: expected %s, actual %s", i, expectedRel, actualRel));
    }
  }

  // A rigid transform rotates both the heading and the field-relative velocity/acceleration by the
  // same amount, so the velocity and acceleration in each sample's heading frame are invariant.
  // This would fail if transformBy/relativeTo rotated the pose but not the velocity/acceleration.
  void testSameRobotRelativeMotion(
      List<? extends HolonomicSample> statesA, List<? extends HolonomicSample> statesB) {
    assertEquals(statesA.size(), statesB.size());
    for (int i = 0; i < statesA.size(); i++) {
      var a = statesA.get(i);
      var b = statesB.get(i);

      var velocityA = a.velocity.toRobotRelative(a.pose.getRotation());
      var velocityB = b.velocity.toRobotRelative(b.pose.getRotation());
      assertEquals(velocityA.vx, velocityB.vx, 1e-9);
      assertEquals(velocityA.vy, velocityB.vy, 1e-9);
      assertEquals(velocityA.omega, velocityB.omega, 1e-9);

      var accelerationA = a.acceleration.toRobotRelative(a.pose.getRotation());
      var accelerationB = b.acceleration.toRobotRelative(b.pose.getRotation());
      assertEquals(accelerationA.ax, accelerationB.ax, 1e-9);
      assertEquals(accelerationA.ay, accelerationB.ay, 1e-9);
      assertEquals(accelerationA.alpha, accelerationB.alpha, 1e-9);
    }
  }

  void testSameShapedHolonomicTrajectory(
      List<? extends HolonomicSample> statesA, List<? extends HolonomicSample> statesB) {
    assertEquals(statesA.size(), statesB.size());
    for (int i = 0; i < statesA.size() - 1; i++) {
      var expectedRel = statesA.get(i + 1).pose.relativeTo(statesA.get(i).pose);
      var actualRel = statesB.get(i + 1).pose.relativeTo(statesB.get(i).pose);

      assertEquals(
          expectedRel,
          actualRel,
          String.format(
              "pose mismatch at index %d: expected %s, actual %s", i, expectedRel, actualRel));
    }
  }

  // Wheel velocities are robot-relative, so they're invariant under rigid transforms
  void testSameWheelVelocities(List<DifferentialSample> statesA, List<DifferentialSample> statesB) {
    assertEquals(statesA.size(), statesB.size());
    for (int i = 0; i < statesA.size(); i++) {
      assertEquals(statesA.get(i).leftVelocity, statesB.get(i).leftVelocity);
      assertEquals(statesA.get(i).rightVelocity, statesB.get(i).rightVelocity);
    }
  }
}
