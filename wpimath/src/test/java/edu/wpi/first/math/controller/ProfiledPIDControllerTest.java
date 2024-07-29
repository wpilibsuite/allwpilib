// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.trajectory.TrapezoidProfile;
import org.junit.jupiter.api.Test;

class ProfiledPIDControllerTest {
  @Test
  void testStartFromNonZeroPosition() {
    ProfiledPIDController controller =
        new ProfiledPIDController(1.0, 0.0, 0.0, new TrapezoidProfile.Constraints(1.0, 1.0));

    controller.reset(20);

    assertEquals(0.0, controller.calculate(20), 0.05);
  }
}
