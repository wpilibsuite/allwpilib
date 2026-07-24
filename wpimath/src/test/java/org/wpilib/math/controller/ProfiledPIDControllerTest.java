// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.trajectory.TrapezoidProfile;

class ProfiledPIDControllerTest {
  @Test
  void testStartFromNonZeroPosition() {
    ProfiledPIDController controller =
        new ProfiledPIDController(1.0, 0.0, 0.0, new TrapezoidProfile.Constraints(1.0, 1.0));

    controller.reset(20);

    assertEquals(0.0, controller.calculate(20), 0.05);
  }

  @Test
  void getNextSetpointTest() {
    ProfiledPIDController controller =
        new ProfiledPIDController(1.0, 0.0, 0.0, new TrapezoidProfile.Constraints(1.0, 1.0));

    // Advance the profile one step toward the goal.
    controller.calculate(0.0, 5.0);

    TrapezoidProfile.State next = controller.getNextSetpoint();

    // The next setpoint should equal the setpoint produced by the following calculate() call.
    controller.calculate(controller.getSetpoint().position);

    assertEquals(next.position, controller.getSetpoint().position, 1e-9);
    assertEquals(next.velocity, controller.getSetpoint().velocity, 1e-9);
  }
}
