// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.spline;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.spline.SplineParameterizer.MalformedSplineException;
import java.util.List;
import org.junit.jupiter.api.Test;

class QuinticHermiteSplineTest {
  private static final double kMaxDx = 0.127;
  private static final double kMaxDy = 0.00127;
  private static final double kMaxDtheta = 0.0872;

  private void run(Pose2d a, Pose2d b) {
    // Start the timer.
    // var start = System.nanoTime();

    // Generate and parameterize the spline.
    var spline = SplineHelper.getQuinticSplinesFromWaypoints(List.of(a, b))[0];
    var poses = SplineParameterizer.parameterize(spline);

    // End the timer.
    // var end = System.nanoTime();

    // Calculate the duration (used when benchmarking)
    // var durationMicroseconds = (end - start) / 1000.0;

    for (int i = 0; i < poses.size() - 1; i++) {
      var p0 = poses.get(i);
      var p1 = poses.get(i + 1);

      // Make sure the twist is under the tolerance defined by the Spline class.
      var twist = p0.poseMeters.log(p1.poseMeters);
      assertAll(
          () -> assertTrue(Math.abs(twist.dx) < kMaxDx),
          () -> assertTrue(Math.abs(twist.dy) < kMaxDy),
          () -> assertTrue(Math.abs(twist.dtheta) < kMaxDtheta));
    }

    // Check first point
    assertAll(
        () -> assertEquals(a.getX(), poses.get(0).poseMeters.getX(), 1E-9),
        () -> assertEquals(a.getY(), poses.get(0).poseMeters.getY(), 1E-9),
        () ->
            assertEquals(
                a.getRotation().getRadians(),
                poses.get(0).poseMeters.getRotation().getRadians(),
                1E-9));

    // Check last point
    assertAll(
        () -> assertEquals(b.getX(), poses.get(poses.size() - 1).poseMeters.getX(), 1E-9),
        () -> assertEquals(b.getY(), poses.get(poses.size() - 1).poseMeters.getY(), 1E-9),
        () ->
            assertEquals(
                b.getRotation().getRadians(),
                poses.get(poses.size() - 1).poseMeters.getRotation().getRadians(),
                1E-9));
  }

  @Test
  void testStraightLine() {
    run(Pose2d.kZero, new Pose2d(3, 0, Rotation2d.kZero));
  }

  @Test
  void testSimpleSCurve() {
    run(Pose2d.kZero, new Pose2d(1, 1, Rotation2d.kZero));
  }

  @Test
  void testSquiggly() {
    run(new Pose2d(0, 0, Rotation2d.kCCW_Pi_2), new Pose2d(-1, 0, Rotation2d.kCCW_Pi_2));
  }

  @Test
  void testMalformed() {
    assertThrows(
        MalformedSplineException.class,
        () -> run(new Pose2d(0, 0, Rotation2d.kZero), new Pose2d(1, 0, Rotation2d.kPi)));
    assertThrows(
        MalformedSplineException.class,
        () ->
            run(new Pose2d(10, 10, Rotation2d.kCCW_Pi_2), new Pose2d(10, 11, Rotation2d.kCW_Pi_2)));
  }
}
