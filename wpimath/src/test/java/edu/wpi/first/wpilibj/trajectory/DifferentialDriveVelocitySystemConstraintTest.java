/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.controller.SimpleMotorFeedforward;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveWheelSpeeds;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.trajectory.constraint.DifferentialDriveVelocitySystemConstraint;
import edu.wpi.first.wpilibj.trajectory.constraint.DifferentialDriveVoltageConstraint;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;

class DifferentialDriveVelocitySystemConstraintTest {
  @SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops"})
  @Test
  void testDifferentialDriveVelocitySystemConstraint() {
    double maxVoltage = 10;

    // Pick an unreasonably large kA to ensure the constraint has to do some work
    var system = LinearSystemId.identifyDrivetrainSystem(1, 3, 1, 3);
    var kinematics = new DifferentialDriveKinematics(0.5);
    //    var constraint = new DifferentialDriveVelocitySystemConstraint(system,
    //          kinematics,
    //          maxVoltage);

    var constraint = new DifferentialDriveVoltageConstraint(
        new SimpleMotorFeedforward(0, 1, 3), kinematics, 10);

    Trajectory trajectory = TrajectoryGeneratorTest.getTrajectory(
          Collections.singletonList(constraint));
    //    Trajectory trajectory = TrajectoryGenerator.generateTrajectory(
    //      List.of(new Pose2d(), new Pose2d(new Translation2d(10, 10),
    //          Rotation2d.fromDegrees(90))),
    //      new TrajectoryConfig(300000000000.0, 300000000000.00).addConstraint(constraint)
    //    );

    var duration = trajectory.getTotalTimeSeconds();
    var t = 0.0;
    var dt = 0.02;
    var previousSpeeds = new DifferentialDriveWheelSpeeds(0, 0);

    List<Double> vleft = new ArrayList<>();
    List<Double> vright = new ArrayList<>();

    System.out.println("time, uleft, uright, vleft, vright, vx, max_vleft, curvature");

    while (t < duration) {
      var point = trajectory.sample(t);
      var chassisSpeeds = new ChassisSpeeds(
            point.velocityMetersPerSecond, 0,
            point.velocityMetersPerSecond * point.curvatureRadPerMeter
      );

      var wheelSpeeds = kinematics.toWheelSpeeds(chassisSpeeds);

      var x = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(wheelSpeeds.leftMetersPerSecond,
            wheelSpeeds.rightMetersPerSecond);

      var leftAccel = (wheelSpeeds.leftMetersPerSecond - previousSpeeds.leftMetersPerSecond) / dt;
      var rightAccel = (wheelSpeeds.rightMetersPerSecond
            - previousSpeeds.rightMetersPerSecond) / dt;
      var xDot = VecBuilder.fill(leftAccel, rightAccel);

      var u = (system.getB().inv()).times(xDot.minus(system.getA().times(x)));

      double left = u.get(0, 0);
      double right = u.get(1, 0);

      vleft.add(left);
      vright.add(right);

      t += dt;
      previousSpeeds = wheelSpeeds;

      System.out.println(String.format("%s, %s, %s, %s, %s, %s, %s, %s", t, u.get(0, 0),
          u.get(1, 0),
          wheelSpeeds.leftMetersPerSecond, wheelSpeeds.rightMetersPerSecond,
          chassisSpeeds.vxMetersPerSecond, constraint
          .getMaxVelocityMetersPerSecond(point.poseMeters, point.curvatureRadPerMeter,
          point.velocityMetersPerSecond), point.curvatureRadPerMeter));

      //      System.out.println(left + ", " + right);
      //      assertTrue((-10.1 <= left) && (left <= 10.1));
      //      assertTrue((-10.1 <= right) && (right <= 10.1));

    }
    //var c = new XYChartBuilder().build();
    //c.addSeries("left voltage", vleft);
    //c.addSeries("right voltage", vright);
    // new SwingWrapper<>(c).displayChart();
    // try {
    //  Thread.sleep(1000000000);
    // } catch (InterruptedException e) {
    // }
  }

  @Test
  void testEndpointHighCurvature() {
    double maxVoltage = 10;

    var system = LinearSystemId.identifyDrivetrainSystem(1, 3, 1, 3);

    // Large trackwidth - need to test with radius of curvature less than half of trackwidth
    var kinematics = new DifferentialDriveKinematics(3);
    var constraint = new DifferentialDriveVelocitySystemConstraint(system,
          kinematics,
          maxVoltage);

    var config = new TrajectoryConfig(12, 12).addConstraint(constraint);

    // Radius of curvature should be ~1 meter.
    assertDoesNotThrow(() -> TrajectoryGenerator.generateTrajectory(
          new Pose2d(1, 0, Rotation2d.fromDegrees(90)),
          new ArrayList<Translation2d>(),
          new Pose2d(0, 1, Rotation2d.fromDegrees(180)),
          config));

    assertDoesNotThrow(() -> TrajectoryGenerator.generateTrajectory(
          new Pose2d(0, 1, Rotation2d.fromDegrees(180)),
          new ArrayList<Translation2d>(),
          new Pose2d(1, 0, Rotation2d.fromDegrees(90)),
          config.setReversed(true)));

  }

  @Test
  @SuppressWarnings("LocalVariableName")
  void testGraph() throws IOException {
    double maxVoltage = 10;

    // Pick an unreasonably large kA to ensure the constraint has to do some work
    var system = LinearSystemId.identifyDrivetrainSystem(1, 3, 1, 3);
    var kinematics = new DifferentialDriveKinematics(0.5);

    var constraint = new DifferentialDriveVelocitySystemConstraint(system, kinematics, maxVoltage);

    var velocity = 12.0;
    var curvature = 10.0;

    var wheelSpeeds = kinematics.toWheelSpeeds(new ChassisSpeeds(velocity, 0,
        velocity
        * curvature));

    var x = VecBuilder.fill(wheelSpeeds.leftMetersPerSecond,
        wheelSpeeds.rightMetersPerSecond);

    var writer = new BufferedWriter(new FileWriter(Path.of("D:\\Documents\\out.csv").toString()));

    System.out.println(x);

    // iterate by steps of 1 over the voltage range
    for (int uLeft = -10; uLeft <= 10; uLeft++) {
      for (int uRight = -10; uRight <= 10; uRight++) {
        var u = VecBuilder.fill(uLeft, uRight);
        var xdot = system.getA().times(x).plus(system.getB().times(u));
        writer.write(String.format("%s,%s,%s", uLeft, uRight, (xdot.get(0, 0)
            + xdot.get(1, 0)) / 2.0));
        writer.newLine();
        writer.flush();
      }
    }
  }
}
