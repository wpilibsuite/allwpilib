// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.snippets.analogtrigger;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.AnalogTrigger;
import edu.wpi.first.wpilibj.TimedRobot;

/**
 * DigitalInput snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/digital-inputs-software.html
 */
public class Robot extends TimedRobot {
  // Initializes an AnalogTrigger on port 0
  AnalogTrigger m_trigger0 = new AnalogTrigger(0);
  // Initializes an AnalogInput on port 1
  AnalogInput m_input = new AnalogInput(1);

  // Initializes an AnalogTrigger using the above input
  AnalogTrigger m_trigger1 = new AnalogTrigger(m_input);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // Enables 2-bit oversampling
    m_input.setAverageBits(2);
    // Sets the trigger to enable at a raw value of 3500, and disable at a value of 1000
    m_trigger0.setLimitsRaw(1000, 3500);
    // Sets the trigger to enable at a voltage of 4 volts, and disable at a value of 1.5 volts
    m_trigger0.setLimitsVoltage(1.5, 4);

    m_trigger1.getTriggerState();
  }
}
