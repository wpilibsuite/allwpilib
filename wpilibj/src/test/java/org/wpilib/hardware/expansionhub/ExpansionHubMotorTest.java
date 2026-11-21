// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.expansionhub;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.units.Units.Volts;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.expansionhub.ExpansionHubMotor.FollowDirection;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.motor.MotorController;

class ExpansionHubMotorTest {
  @Test
  void testMotorControllerThrottle() {
    HAL.initialize();
    try (var motor = new ExpansionHubMotor(0, 0)) {
      MotorController controller = motor;
      assertEquals(0, controller.getThrottle());

      for (double throttle : new double[] {0.5, -0.25, 0}) {
        controller.setThrottle(throttle);
        assertEquals(throttle, controller.getThrottle());
      }

      controller.setThrottle(0.5);
      motor.setPositionSetpoint(100);
      assertEquals(0.5, controller.getThrottle());
      motor.setVelocitySetpoint(20);
      assertEquals(0.5, controller.getThrottle());
      motor.unfollow();
      assertEquals(0, controller.getThrottle());
    }
  }

  @Test
  void testMotorControllerVoltage() {
    HAL.initialize();
    try (var motor = new ExpansionHubMotor(0, 0)) {
      MotorController controller = motor;
      controller.setThrottle(0.5);
      controller.setVoltage(-6.0);
      assertEquals(0.5, controller.getThrottle());

      controller.setVoltage(Volts.of(-3.0));
      assertEquals(0.5, controller.getThrottle());
    }
  }

  @Test
  void testMotorControllerInverted() {
    HAL.initialize();
    try (var motor = new ExpansionHubMotor(0, 0)) {
      MotorController controller = motor;
      assertFalse(controller.getInverted());

      controller.setInverted(true);
      assertTrue(controller.getInverted());
      controller.setThrottle(0.5);
      assertEquals(0.5, controller.getThrottle());

      motor.setReversed(false);
      assertFalse(controller.getInverted());
      motor.setReversed(true);
      assertTrue(controller.getInverted());
      controller.setInverted(false);
      assertFalse(controller.getInverted());
    }
  }

  @Test
  void testFollower() {
    HAL.initialize();
    try (ExpansionHubMotor motor0 = new ExpansionHubMotor(0, 0);
        ExpansionHubMotor motor1 = new ExpansionHubMotor(0, 1);
        ExpansionHubMotor motor2 = new ExpansionHubMotor(0, 2); ) {
      motor1.follow(motor2, FollowDirection.Opposed);
      motor2.follow(motor0, FollowDirection.Opposed);
    }
  }

  @Test
  void testFollowerCycle() {
    HAL.initialize();
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
