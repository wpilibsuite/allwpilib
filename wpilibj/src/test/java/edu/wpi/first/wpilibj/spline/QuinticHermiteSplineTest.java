/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.spline;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;

class QuinticHermiteSplineTest {
  private double run(Pose2d a, Pose2d b) {
    var start = System.nanoTime();
    var spline = SplineHelper.getQuinticSplinesFromWaypoints(new Pose2d[]{a, b})[0];
    spline.parameterize();
    var end = System.nanoTime();

    return (end - start) / 1000.0;
  }

  @Test
  void testStraightLine() {
    double time = 0.0;
    for (int i = 0; i < 100; i++) {
      time += run(new Pose2d(), new Pose2d(3, 0, new Rotation2d()));
    }
    System.out.println("Average Time: " + time / 100.0);
  }

  @Test
  void testSimpleSCurve() {
    double time = 0.0;
    for (int i = 0; i < 100; i++) {
      time += run(new Pose2d(), new Pose2d(1, 1, new Rotation2d()));
    }
    System.out.println("Average Time: " + time / 100.0);
  }

  @Test
  void testSquiggly() {
    double time = 0.0;
    for (int i = 0; i < 100; i++) {
      time += run(new Pose2d(0, 0, Rotation2d.fromDegrees(90)),
          new Pose2d(-1, 0, Rotation2d.fromDegrees(90)));
    }
    System.out.println("Average Time: " + time / 100.0);
  }

  @Test
  void printOutSquiggly() {
    var a = new Pose2d(0, 0, Rotation2d.fromDegrees(90.0));
    var b = new Pose2d(-1, 0, Rotation2d.fromDegrees(90.0));
    var spline = SplineHelper.getQuinticSplinesFromWaypoints(new Pose2d[]{a, b})[0];
    var points = spline.parameterize();
    for (var point : points) {
      System.out.println(point.pose.getTranslation().getX() + ", "
          + point.pose.getTranslation().getY());
    }
  }
}
