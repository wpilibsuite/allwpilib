/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory;

import java.util.ArrayList;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Transform2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.trajectory.constraint.CentripetalAccelerationConstraint;
import edu.wpi.first.wpilibj.trajectory.constraint.TrajectoryConstraint;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertTrue;

class TrajectoryGeneratorTest {
  private static double feetToMeters(double feet) {
    return feet * 0.3048;
  }

  @Test
  void testObeysConstraints() {
    double startVelocity = 0;
    double endVelocity = 0;
    double maxVelocity = feetToMeters(12.0);
    double maxAccel = feetToMeters(12);
    double maxCentripetalAccel = feetToMeters(7);

    var constraints = new ArrayList<TrajectoryConstraint>();
    constraints.add(new CentripetalAccelerationConstraint(maxCentripetalAccel));

    // 2018 cross scale auto waypoints.
    var sideStart = new Pose2d(feetToMeters(1.54), feetToMeters(23.23),
        Rotation2d.fromDegrees(-180));
    var crossScale = new Pose2d(feetToMeters(23.7), feetToMeters(6.8),
        Rotation2d.fromDegrees(-160));

    var waypoints = new ArrayList<Pose2d>();
    waypoints.add(sideStart);
    waypoints.add((sideStart.plus(
        new Transform2d(new Translation2d(feetToMeters(-13), feetToMeters(0)),
            new Rotation2d()))));
    waypoints.add((sideStart.plus(
        new Transform2d(new Translation2d(feetToMeters(-19.5), feetToMeters(5)),
            Rotation2d.fromDegrees(-90)))));
    waypoints.add(crossScale);

    var start = System.nanoTime();
    var trajectory = TrajectoryGenerator.generateTrajectory(
        waypoints,
        constraints,
        startVelocity,
        endVelocity,
        maxVelocity,
        maxAccel,
        true
    );
    var end = System.nanoTime();

    System.out.println("Generation Took: " + (end - start) / 1000.0 + " microseconds");

    double t = 0.0;
    double dt = 0.02;
    double duration = trajectory.getTotalTimeSeconds();

    while (t < duration) {
      var point = trajectory.sample(t);
      double a_c = Math.pow(point.velocityMetersPerSecond, 2) * point.curvatureRadPerMeter;

      assertAll(
          () -> assertTrue(Math.abs(a_c) <= maxCentripetalAccel + 0.05),
          () -> assertTrue(Math.abs(point.velocityMetersPerSecond) < maxVelocity + feetToMeters(0.01)),
          () -> assertTrue(Math.abs(point.accelerationMetersPerSecondSq) < maxAccel + feetToMeters(0.01))
      );

      var pose = point.poseMeters;

//      System.out.println(point.curvatureRadPerMeter);
      System.out.println(pose.getTranslation().getX() + ", " + pose.getTranslation().getY());
      t += dt;
    }
  }
}
