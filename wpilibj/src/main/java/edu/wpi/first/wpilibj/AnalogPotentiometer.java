// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Class for reading analog potentiometers. Analog potentiometers read in an analog voltage that
 * corresponds to a position. The position is in whichever units you choose, by way of the scaling
 * and offset constants passed to the constructor.
 */
public class AnalogPotentiometer implements Sendable, AutoCloseable {
  private AnalogInput m_analogInput;
  private boolean m_initAnalogInput;
  private double m_fullRange;
  private double m_offset;

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>Use the fullRange and offset values so that the output produces meaningful values. I.E: you
   * have a 270 degree potentiometer and you want the output to be degrees with the halfway point as
   * 0 degrees. The fullRange value is 270.0(degrees) and the offset is -135.0 since the halfway
   * point after scaling is 135 degrees. This will calculate the result from the fullRange times the
   * fraction of the supply voltage, plus the offset.
   *
   * @param channel The analog input channel this potentiometer is plugged into. 0-3 are on-board
   *     and 4-7 are on the MXP port.
   * @param fullRange The scaling to multiply the fraction by to get a meaningful unit.
   * @param offset The offset to add to the scaled value for controlling the zero value
   */
  public AnalogPotentiometer(final int channel, double fullRange, double offset) {
    this(new AnalogInput(channel), fullRange, offset);
    m_initAnalogInput = true;
    SendableRegistry.addChild(this, m_analogInput);
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>Use the fullRange and offset values so that the output produces meaningful values. I.E: you
   * have a 270 degree potentiometer and you want the output to be degrees with the halfway point as
   * 0 degrees. The fullRange value is 270.0(degrees) and the offset is -135.0 since the halfway
   * point after scaling is 135 degrees. This will calculate the result from the fullRange times the
   * fraction of the supply voltage, plus the offset.
   *
   * @param input The {@link AnalogInput} this potentiometer is plugged into.
   * @param fullRange The angular value (in desired units) representing the full 0-5V range of the
   *     input.
   * @param offset The angular value (in desired units) representing the angular output at 0V.
   */
  public AnalogPotentiometer(final AnalogInput input, double fullRange, double offset) {
    SendableRegistry.addLW(this, "AnalogPotentiometer", input.getChannel());
    m_analogInput = input;
    m_initAnalogInput = false;

    m_fullRange = fullRange;
    m_offset = offset;
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>Use the scale value so that the output produces meaningful values. I.E: you have a 270
   * degree potentiometer and you want the output to be degrees with the starting point as 0
   * degrees. The scale value is 270.0(degrees).
   *
   * @param channel The analog input channel this potentiometer is plugged into. 0-3 are on-board
   *     and 4-7 are on the MXP port.
   * @param scale The scaling to multiply the voltage by to get a meaningful unit.
   */
  public AnalogPotentiometer(final int channel, double scale) {
    this(channel, scale, 0);
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>Use the fullRange and offset values so that the output produces meaningful values. I.E: you
   * have a 270 degree potentiometer and you want the output to be degrees with the starting point
   * as 0 degrees. The scale value is 270.0(degrees).
   *
   * @param input The {@link AnalogInput} this potentiometer is plugged into.
   * @param scale The scaling to multiply the voltage by to get a meaningful unit.
   */
  public AnalogPotentiometer(final AnalogInput input, double scale) {
    this(input, scale, 0);
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>The potentiometer will return a value between 0 and 1.0.
   *
   * @param channel The analog input channel this potentiometer is plugged into. 0-3 are on-board
   *     and 4-7 are on the MXP port.
   */
  public AnalogPotentiometer(final int channel) {
    this(channel, 1, 0);
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>The potentiometer will return a value between 0 and 1.0.
   *
   * @param input The {@link AnalogInput} this potentiometer is plugged into.
   */
  public AnalogPotentiometer(final AnalogInput input) {
    this(input, 1, 0);
  }

  /**
   * Get the current reading of the potentiometer.
   *
   * @return The current position of the potentiometer.
   */
  public double get() {
    if (m_analogInput == null) {
      return m_offset;
    }
    return (m_analogInput.getAverageVoltage() / RobotController.getVoltage5V()) * m_fullRange
        + m_offset;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    if (m_analogInput != null) {
      builder.setSmartDashboardType("Analog Input");
      builder.addDoubleProperty("Value", this::get, null);
    }
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    if (m_initAnalogInput) {
      m_analogInput.close();
      m_analogInput = null;
      m_initAnalogInput = false;
    }
  }
}
