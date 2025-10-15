// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.digitalinput;

import org.wpilib.hardware.discrete.DigitalInput;
import org.wpilib.opmode.TimedRobot;

/**
 * DigitalInput snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/digital-inputs-software.html
 */
public class Robot extends TimedRobot {
  // Initializes a DigitalInput on DIO 0
  DigitalInput m_input = new DigitalInput(0);

  @Override
  public void teleopPeriodic() {
    // Gets the value of the digital input.  Returns true if the circuit is open.
    m_input.get();
  }
}
