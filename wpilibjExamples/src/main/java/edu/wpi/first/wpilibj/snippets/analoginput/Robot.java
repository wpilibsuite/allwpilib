// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.snippets.analoginput;

import edu.wpi.first.hal.AccumulatorResult;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * AnalogInput snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/analog-inputs-software.html
 */
public class Robot extends TimedRobot {
  // Initializes an AnalogInput on port 0
  AnalogInput m_analog = new AnalogInput(0);

  // Instantiate an AccumulatorResult object to hold the matched measurements
  AccumulatorResult m_result = new AccumulatorResult();

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

    // Sets the initial value of the accumulator to 0
    // This is the "starting point" from which the value will change over time
    m_analog.setAccumulatorInitialValue(0);
    // Sets the "center" of the accumulator to 0.  This value is subtracted from
    // all measured values prior to accumulation.
    m_analog.setAccumulatorCenter(0);
    // Returns the number of accumulated samples since the accumulator was last started/reset
    m_analog.getAccumulatorCount();
    // Returns the value of the accumulator.  Return type is long.
    m_analog.getAccumulatorValue();
    // Resets the accumulator to the initial value
    m_analog.resetAccumulator();

    // Fill the AccumulatorResult with the matched measurements
    m_analog.getAccumulatorOutput(m_result);
    // Read the values from the AccumulatorResult
    long count = m_result.count;
    long value = m_result.value;

    SmartDashboard.putNumber("Accumulator Count", count);
    SmartDashboard.putNumber("Accumulator Value", value);
  }
}
