// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.servotest;

import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.servo.RotaryServo;

// This test verifies that user code (which is not in `edu.wpi.first.wpilibj.servo` package) is able
// to subclass the RotaryServo class and fields such as m_pwm are correctly visible. This is why
// the package is called `edu.wpi.first.wpilibj.servotest` and not `.servo` as would be consistent.

public class MockRotaryServo extends RotaryServo {
  public MockRotaryServo(final int channel) {
    super("MockRotaryServo", channel, 0, Units.degreesToRadians(180));

    m_pwm.setBounds(5.0, 0, 0, 0, 1.0);
  }
}
