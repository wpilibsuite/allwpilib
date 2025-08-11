// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.profile.TrapezoidProfile;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class HolonomicDriveControllerTest {

  @Test
  void testDoesNotRotateUnnecessarily() {
    var controller =
        new HolonomicDriveController(
            new PIDController(1, 0, 0),
            new PIDController(1, 0, 0),
            new ProfiledPIDController(1, 0, 0, new TrapezoidProfile.Constraints(4, 2)));

    ChassisSpeeds speeds =
        controller.calculate(
            new Pose2d(0, 0, new Rotation2d(1.57)), Pose2d.kZero, 0, new Rotation2d(1.57));

    assertEquals(0.0, speeds.omega);
  }
}
