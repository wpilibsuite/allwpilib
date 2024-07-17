// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.dutycycleencoder;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.wpilibj.DutyCycleEncoder;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/** This example shows how to use a duty cycle encoder for devices such as an arm or elevator. */
public class Robot extends TimedRobot {
  private final DutyCycleEncoder m_dutyCycleEncoder;
  private static final double m_fullRange = 1.3;
  private static final double m_expectedZero = 0;

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // 2nd parameter is the range of values. This sensor will output between
    // 0 and the passed in value.
    // 3rd parameter is the the physical value where you want "0" to be. How
    // to measure this is fairly easy. Set the value to 0, place the mechanism
    // where you want "0" to be, and observe the value on the dashboard, That
    // is the value to enter for the 3rd parameter.
    m_dutyCycleEncoder = new DutyCycleEncoder(0, m_fullRange, m_expectedZero);

    // If you know the frequency of your sensor, uncomment the following
    // method, and set the method to the frequency of your sensor.
    // This will result in more stable readings from the sensor.
    // Do note that occasionally the datasheet cannot be trusted
    // and you should measure this value. You can do so with either
    // an oscilloscope, or by observing the "Frequency" output
    // on the dashboard while running this sample. If you find
    // the value jumping between the 2 values, enter halfway between
    // those values. This number doesn't have to be perfect,
    // just having a fairly close value will make the output readings
    // much more stable.
    m_dutyCycleEncoder.setAssumedFrequency(967.8);
  }

  @Override
  public void robotPeriodic() {
    // Connected can be checked, and uses the frequency of the encoder
    boolean connected = m_dutyCycleEncoder.isConnected();

    // Duty Cycle Frequency in Hz
    int frequency = m_dutyCycleEncoder.getFrequency();

    // Output of encoder
    double output = m_dutyCycleEncoder.get();

    // By default, the output will wrap around to the full range value
    // when the sensor goes below 0. However, for moving mechanisms this
    // is not usually ideal, as if 0 is set to a hard stop, its still
    // possible for the sensor to move slightly past. If this happens
    // The sensor will assume its now at the furthest away position,
    // which control algorithms might not handle correctly. Therefore
    // it can be a good idea to slightly shift the output so the sensor
    // can go a bit negative before wrapping. Usually 10% or so is fine.
    // This does not change where "0" is, so no calibration numbers need
    // to be changed.
    double percentOfRange = m_fullRange * 0.1;
    double shiftedOutput =
        MathUtil.inputModulus(output, 0 - percentOfRange, m_fullRange - percentOfRange);

    SmartDashboard.putBoolean("Connected", connected);
    SmartDashboard.putNumber("Frequency", frequency);
    SmartDashboard.putNumber("Output", output);
    SmartDashboard.putNumber("ShiftedOutput", shiftedOutput);
  }
}
