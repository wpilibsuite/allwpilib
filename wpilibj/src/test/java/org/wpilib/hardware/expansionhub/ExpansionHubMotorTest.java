// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.expansionhub;

import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.expansionhub.ExpansionHubMotor.FollowDirection;
import org.wpilib.hardware.hal.HAL;

class ExpansionHubMotorTest {
  @Test
  void testFollower() {
    HAL.initialize(500, 0);
    try (ExpansionHubMotor motor0 = new ExpansionHubMotor(0, 0);
        ExpansionHubMotor motor1 = new ExpansionHubMotor(0, 1);
        ExpansionHubMotor motor2 = new ExpansionHubMotor(0, 2); ) {
      motor1.follow(motor2, FollowDirection.Opposed);
      motor2.follow(motor0, FollowDirection.Opposed);
    }
  }

  @Test
  void testFollowerCycle() {
    HAL.initialize(500, 0);
    try (ExpansionHubMotor motor0 = new ExpansionHubMotor(0, 0);
        ExpansionHubMotor motor1 = new ExpansionHubMotor(0, 1);
        ExpansionHubMotor motor2 = new ExpansionHubMotor(0, 2); ) {
      motor1.follow(motor2, FollowDirection.Opposed);
      motor2.follow(motor0, FollowDirection.Opposed);
      assertThrows(
          IllegalStateException.class, () -> motor0.follow(motor1, FollowDirection.Opposed));
    }
  }
}
