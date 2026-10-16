// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.math.util.UnitConversions.feetToMeters;
import static org.wpilib.math.util.UnitConversions.inchesToMeters;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.trajectory.constraint.DifferentialDriveKinematicsConstraint;

class DifferentialDriveKinematicsConstraintTest {
  @Test
  void testDifferentialDriveKinematicsConstraint() {
    double maxVelocity = feetToMeters(12.0); // 12 feet per second
    var kinematics = new DifferentialDriveKinematics(inchesToMeters(27));
    var constraint = new DifferentialDriveKinematicsConstraint(kinematics, maxVelocity);

    Trajectory<DrivetrainSplineSample> trajectory =
        DrivetrainSplineTrajectoryGenerator.generate(
            List.of(new Pose2d(0, 0, Rotation2d.ZERO), new Pose2d(1, 0, Rotation2d.ZERO)),
            new TrajectoryConfig(1, 1).addConstraint(constraint));
    var duration = trajectory.duration;

    for (double t = 0; t < duration; t += 0.02) {
      var point = trajectory.sampleAt(t);
      var chassisVelocities =
          new ChassisVelocities(
              point.forwardVelocity(), 0, point.forwardVelocity() * point.curvature);

      var wheelVelocities = kinematics.toWheelVelocities(chassisVelocities);

      assertAll(
          () -> assertTrue(wheelVelocities.left <= maxVelocity + 0.05),
          () -> assertTrue(wheelVelocities.right <= maxVelocity + 0.05));
    }
  }
}
