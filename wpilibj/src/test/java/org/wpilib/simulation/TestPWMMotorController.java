// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.hardware.motor.PWMMotorController;

/** Test-only concrete PWM motor controller. */
@SuppressWarnings("PMD.TestClassWithoutTestCases")
final class TestPWMMotorController extends PWMMotorController {
  @SuppressWarnings("this-escape")
  TestPWMMotorController(int channel) {
    super(channel);

    setBoundsMicroseconds(2003, 1550, 1500, 1460, 999);
    m_pwm.setOutputPeriod(5);
    setThrottle(0.0);
  }
}
