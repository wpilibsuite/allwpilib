// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.analoginput;

import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.discrete.AnalogInput;

/**
 * AnalogInput snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/analog-inputs-software.html
 */
public class Robot extends TimedRobot {
  // Initializes an AnalogInput on port 0
  AnalogInput m_analog = new AnalogInput(0);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // Sets the AnalogInput to 4-bit oversampling.  16 samples will be added together.
    // Thus, the reported values will increase by about a factor of 16, and the update
    // rate will decrease by a similar amount.
    m_analog.setOversampleBits(4);

    // Sets the AnalogInput to 4-bit averaging.  16 samples will be averaged together.
    // The update rate will decrease by a factor of 16.
    m_analog.setAverageBits(4);
  }

  @Override
  public void teleopPeriodic() {
    // Gets the raw instantaneous measured value from the analog input, without
    // applying any calibration and ignoring oversampling and averaging
    // settings.
    m_analog.getValue();

    // Gets the instantaneous measured voltage from the analog input.
    // Oversampling and averaging settings are ignored
    m_analog.getVoltage();

    // Gets the averaged value from the analog input.  The value is not
    // rescaled, but oversampling and averaging are both applied.
    m_analog.getAverageValue();

    // Gets the averaged voltage from the analog input.  Rescaling,
    // oversampling, and averaging are all applied.
    m_analog.getAverageVoltage();
  }
}
