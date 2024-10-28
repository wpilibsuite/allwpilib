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
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.spline.SplineParameterizer.MalformedSplineException;
import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.Test;

class CubicHermiteSplineTest {
  private static final double kMaxDx = 0.127;
  private static final double kMaxDy = 0.00127;
  private static final double kMaxDtheta = 0.0872;

  private void run(Pose2d a, List<Translation2d> waypoints, Pose2d b) {
    // Start the timer.
    // var start = System.nanoTime();

    // Generate and parameterize the spline.
    var controlVectors =
        SplineHelper.getCubicControlVectorsFromWaypoints(
            a, waypoints.toArray(new Translation2d[0]), b);
    var splines =
        SplineHelper.getCubicSplinesFromControlVectors(
            controlVectors[0], waypoints.toArray(new Translation2d[0]), controlVectors[1]);

    var poses = new ArrayList<PoseWithCurvature>();

    poses.add(splines[0].getPoint(0.0).get());

    for (var spline : splines) {
      poses.addAll(SplineParameterizer.parameterize(spline));
    }

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

    // Check interior waypoints
    boolean interiorsGood = true;
    for (var waypoint : waypoints) {
      boolean found = false;
      for (var state : poses) {
        if (waypoint.getDistance(state.poseMeters.getTranslation()) == 0) {
          found = true;
        }
      }
      interiorsGood &= found;
    }

    assertTrue(interiorsGood);

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
    run(Pose2d.kZero, new ArrayList<>(), new Pose2d(3, 0, Rotation2d.kZero));
  }

  @Test
  void testSCurve() {
    var start = new Pose2d(0, 0, Rotation2d.kCCW_Pi_2);
    ArrayList<Translation2d> waypoints = new ArrayList<>();
    waypoints.add(new Translation2d(1, 1));
    waypoints.add(new Translation2d(2, -1));
    var end = new Pose2d(3, 0, Rotation2d.kCCW_Pi_2);

    run(start, waypoints, end);
  }

  @Test
  void testOneInterior() {
    var start = Pose2d.kZero;
    ArrayList<Translation2d> waypoints = new ArrayList<>();
    waypoints.add(new Translation2d(2.0, 0.0));
    var end = new Pose2d(4, 0, Rotation2d.kZero);

    run(start, waypoints, end);
  }

  @Test
  void testWindyPath() {
    final var start = Pose2d.kZero;
    final ArrayList<Translation2d> waypoints = new ArrayList<>();
    waypoints.add(new Translation2d(0.5, 0.5));
    waypoints.add(new Translation2d(0.5, 0.5));
    waypoints.add(new Translation2d(1.0, 0.0));
    waypoints.add(new Translation2d(1.5, 0.5));
    waypoints.add(new Translation2d(2.0, 0.0));
    waypoints.add(new Translation2d(2.5, 0.5));
    final var end = new Pose2d(3.0, 0.0, Rotation2d.kZero);

    run(start, waypoints, end);
  }

  @Test
  void testMalformed() {
    assertThrows(
        MalformedSplineException.class,
        () ->
            run(
                new Pose2d(0, 0, Rotation2d.kZero),
                new ArrayList<>(),
                new Pose2d(1, 0, Rotation2d.kPi)));
    assertThrows(
        MalformedSplineException.class,
        () ->
            run(
                new Pose2d(10, 10, Rotation2d.kCCW_Pi_2),
                List.of(new Translation2d(10, 10.5)),
                new Pose2d(10, 11, Rotation2d.kCW_Pi_2)));
  }
}
