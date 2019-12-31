/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;

public class ProfiledPIDControllerTest {
  @Test
  @SuppressWarnings("PMD")
  public void testStartFromNonZeroPosition() {
    ProfiledPIDController controller = new ProfiledPIDController(1.0, 0.0, 0.0,
        new TrapezoidProfile.Constraints(1.0, 1.0));

    controller.reset(20);

    Assertions.assertEquals(0.0, controller.calculate(20), 0.05);
  }

}
