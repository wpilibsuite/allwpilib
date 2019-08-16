/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.spline;

import java.util.ArrayList;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;

class CubicHermiteSplineTest {
//  @Test
//  void testStraightLineParameterization() {
//    var points = SplineHelper.getCubicSplinesFromWaypoints(
//        new Pose2d(), new Translation2d[]{}, new Pose2d(3, 0, new Rotation2d()))[0]
//        .parameterize();
//
//    for (var point : points) {
////      System.out.println(point.pose.getTranslation().getX() + ", "
////          + point.pose.getTranslation().getY());
//    }
//  }
//
//  @Test
//  void testCurveParameterization() {
//    var points = SplineHelper.getCubicSplinesFromWaypoints(
//        new Pose2d(), new Translation2d[]{}, new Pose2d(3, 3, new Rotation2d()))[0]
//        .parameterize();
//
//    for (var point : points) {
//      System.out.println(point.pose.getTranslation().getX() + ", "
//          + point.pose.getTranslation().getY());
//    }
//  }


  @Test
  void testPlottingCubicCurve() {
    var a = new Pose2d(0, 0, Rotation2d.fromDegrees(90.0));
    var waypoints = new Translation2d[]{new Translation2d(1, 1), new Translation2d(2, -1)};
    var b = new Pose2d(3, 0, Rotation2d.fromDegrees(90.0));

    var splines = SplineHelper.getCubicSplinesFromWaypoints(a, waypoints, b);
    ArrayList<PoseWithCurvature> points = new ArrayList<>();

    for (var spline : splines) {
      points.addAll(spline.parameterize());
    }

    for (var point : points) {
//      System.out.println(point.pose.getTranslation().getX() + ", "
//          + point.pose.getTranslation().getY());
      System.out.println(point.curvature);
    }
  }
}
