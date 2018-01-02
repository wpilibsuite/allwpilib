/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.interfaces.Potentiometer;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * Class for reading analog potentiometers. Analog potentiometers read in an analog voltage that
 * corresponds to a position. The position is in whichever units you choose, by way of the scaling
 * and offset constants passed to the constructor.
 */
public class AnalogPotentiometer extends SensorBase implements Potentiometer, Sendable {
  private AnalogInput m_analogInput;
  private boolean m_initAnalogInput;
  private double m_fullRange;
  private double m_offset;
  protected PIDSourceType m_pidSource = PIDSourceType.kDisplacement;

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>Use the fullRange and offset values so that the output produces meaningful values. I.E: you
   * have a 270 degree potentiometer and you want the output to be degrees with the halfway point as
   * 0 degrees. The fullRange value is 270.0(degrees) and the offset is -135.0 since the halfway
   * point after scaling is 135 degrees. This will calculate the result from the fullRange times
   * the fraction of the supply voltage, plus the offset.
   *
   * @param channel   The analog channel this potentiometer is plugged into.
   * @param fullRange The scaling to multiply the fraction by to get a meaningful unit.
   * @param offset    The offset to add to the scaled value for controlling the zero value
   */
  public AnalogPotentiometer(final int channel, double fullRange, double offset) {
    this(new AnalogInput(channel), fullRange, offset);
    m_initAnalogInput = true;
    addChild(m_analogInput);
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>Use the fullRange and offset values so that the output produces meaningful values. I.E: you
   * have a 270 degree potentiometer and you want the output to be degrees with the halfway point as
   * 0 degrees. The fullRange value is 270.0(degrees) and the offset is -135.0 since the halfway
   * point after scaling is 135 degrees. This will calculate the result from the fullRange times
   * the fraction of the supply voltage, plus the offset.
   *
   * @param input     The {@link AnalogInput} this potentiometer is plugged into.
   * @param fullRange The scaling to multiply the fraction by to get a meaningful unit.
   * @param offset    The offset to add to the scaled value for controlling the zero value
   */
  public AnalogPotentiometer(final AnalogInput input, double fullRange, double offset) {
    setName("AnalogPotentiometer", input.getChannel());
    m_analogInput = input;
    m_initAnalogInput = false;

    m_fullRange = fullRange;
    m_offset = offset;
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>Use the fullRange and offset values so that the output produces meaningful values. I.E: you
   * have a 270 degree potentiometer and you want the output to be degrees with the halfway point as
   * 0 degrees. The fullRange value is 270.0(degrees) and the offset is -135.0 since the halfway
   * point after scaling is 135 degrees.
   *
   * @param channel The analog channel this potentiometer is plugged into.
   * @param scale   The scaling to multiply the voltage by to get a meaningful unit.
   */
  public AnalogPotentiometer(final int channel, double scale) {
    this(channel, scale, 0);
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>Use the fullRange and offset values so that the output produces meaningful values. I.E: you
   * have a 270 degree potentiometer and you want the output to be degrees with the halfway point as
   * 0 degrees. The fullRange value is 270.0(degrees) and the offset is -135.0 since the halfway
   * point after scaling is 135 degrees.
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
   * @param channel The analog channel this potentiometer is plugged into.
   */
  public AnalogPotentiometer(final int channel) {
    this(channel, 1, 0);
  }

  /**
   * AnalogPotentiometer constructor.
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
  @Override
  public double get() {
    if (m_analogInput == null) {
      return m_offset;
    }
    return (m_analogInput.getVoltage() / RobotController.getVoltage5V()) * m_fullRange + m_offset;
  }

  @Override
  public void setPIDSourceType(PIDSourceType pidSource) {
    if (!pidSource.equals(PIDSourceType.kDisplacement)) {
      throw new IllegalArgumentException("Only displacement PID is allowed for potentiometers.");
    }
    m_pidSource = pidSource;
  }

  @Override
  public PIDSourceType getPIDSourceType() {
    return m_pidSource;
  }

  /**
   * Implement the PIDSource interface.
   *
   * @return The current reading.
   */
  @Override
  public double pidGet() {
    return get();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    if (m_analogInput != null) {
      m_analogInput.initSendable(builder);
    }
  }

  /**
   * Frees this resource.
   */
  @Override
  public void free() {
    super.free();
    if (m_initAnalogInput) {
      m_analogInput.free();
      m_analogInput = null;
      m_initAnalogInput = false;
    }
  }
}
