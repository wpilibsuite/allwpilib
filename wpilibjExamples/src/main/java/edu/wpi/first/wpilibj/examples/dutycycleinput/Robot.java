// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.dutycycleinput;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DutyCycle;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

@SuppressWarnings({"PMD.SingularField"})
public class Robot extends TimedRobot {
  private DigitalInput m_input;
  private DutyCycle m_dutyCycle;

  @Override
  public void robotInit() {
    m_input = new DigitalInput(0);
    m_dutyCycle = new DutyCycle(m_input);
  }

  @Override
  public void robotPeriodic() {
    // Duty Cycle Frequency in Hz
    int frequency = m_dutyCycle.getFrequency();

    // Output of duty cycle, ranging from 0 to 1
    // 1 is fully on, 0 is fully off
    double output = m_dutyCycle.getOutput();

    SmartDashboard.putNumber("Frequency", frequency);
    SmartDashboard.putNumber("Duty Cycle", output);
  }

}
