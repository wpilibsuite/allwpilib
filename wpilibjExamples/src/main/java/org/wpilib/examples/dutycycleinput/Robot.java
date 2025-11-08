// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.dutycycleinput;

import org.wpilib.hardware.rotation.DutyCycle;
import org.wpilib.opmode.TimedRobot;
import org.wpilib.smartdashboard.SmartDashboard;

public class Robot extends TimedRobot {
  private final DutyCycle m_dutyCycle;

  public Robot() {
    m_dutyCycle = new DutyCycle(0);
  }

  @Override
  public void robotPeriodic() {
    // Duty Cycle Frequency in Hz
    double frequency = m_dutyCycle.getFrequency();

    // Output of duty cycle, ranging from 0 to 1
    // 1 is fully on, 0 is fully off
    double output = m_dutyCycle.getOutput();

    SmartDashboard.putNumber("Frequency", frequency);
    SmartDashboard.putNumber("Duty Cycle", output);
  }
}
