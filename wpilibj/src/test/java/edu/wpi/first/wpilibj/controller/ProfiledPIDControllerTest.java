/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;

import static org.junit.jupiter.api.Assertions.assertEquals;

class ProfiledPIDControllerTest {
  @Test
  void testStartFromNonZeroPosition() {
    ProfiledPIDController controller = new ProfiledPIDController(1.0, 0.0, 0.0,
        new TrapezoidProfile.Constraints(1.0, 1.0));

    controller.reset(20);

    assertEquals(0.0, controller.calculate(20), 0.05);
  }
}
