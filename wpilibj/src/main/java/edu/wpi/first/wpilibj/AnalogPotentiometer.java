// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.hal.SimDevice.Direction;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.AnalogTriggerOutput.AnalogTriggerType;

/**
 * Class for reading analog potentiometers. Analog potentiometers read in an
 * analog voltage that
 * corresponds to a position. The position is in whichever units you choose, by
 * way of the scaling
 * and offset constants passed to the constructor.
 */
public class AnalogPotentiometer implements Sendable, AutoCloseable {
  private AnalogInput m_analogInput;
  private boolean m_initAnalogInput;
  private double m_fullRange;
  private double m_offset;
  private AnalogTrigger m_rotationTrigger;
  private Counter m_rotationCounter;
  private double m_lastPosition;

  private SimDevice m_simDevice;
  private SimDouble m_simPosition;

  private void init() {
    m_simDevice = SimDevice.create("AnalogPotentiometer", m_analogInput.getChannel());

    if (m_simDevice != null) {
      m_simPosition = m_simDevice.createDouble("Position", Direction.kInput, 0.0);
    }
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>
   * Use the fullRange and offset values so that the output produces meaningful
   * values. I.E: you
   * have a 270 degree potentiometer, and you want the output to be degrees with
   * the halfway point
   * as 0 degrees. The fullRange value is 270.0(degrees) and the offset is -135.0
   * since the halfway
   * point after scaling is 135 degrees. This will calculate the result from the
   * fullRange times the
   * fraction of the supply voltage, plus the offset.
   *
   * @param channel   The analog input channel this potentiometer is plugged into.
   *                  0-3 are on-board
   *                  and 4-7 are on the MXP port.
   * @param fullRange The scaling to multiply the fraction by to get a meaningful
   *                  unit.
   * @param offset    The offset to add to the scaled value for controlling the
   *                  zero value
   */
  @SuppressWarnings("this-escape")
  public AnalogPotentiometer(final int channel, double fullRange, double offset) {
    this(new AnalogInput(channel), fullRange, offset);
    m_initAnalogInput = true;
    SendableRegistry.addChild(this, m_analogInput);
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>
   * Use the fullRange and offset values so that the output produces meaningful
   * values. I.E: you
   * have a 270 degree potentiometer, and you want the output to be degrees with
   * the halfway point
   * as 0 degrees. The fullRange value is 270.0(degrees) and the offset is -135.0
   * since the halfway
   * point after scaling is 135 degrees. This will calculate the result from the
   * fullRange times the
   * fraction of the supply voltage, plus the offset.
   *
   * @param input     The {@link AnalogInput} this potentiometer is plugged into.
   * @param fullRange The angular value (in desired units) representing the full
   *                  0-5V range of the
   *                  input.
   * @param offset    The angular value (in desired units) representing the
   *                  angular output at 0V.
   */
  @SuppressWarnings("this-escape")
  public AnalogPotentiometer(final AnalogInput input, double fullRange, double offset) {
    SendableRegistry.addLW(this, "AnalogPotentiometer", input.getChannel());
    m_analogInput = requireNonNullParam(input, "input", "AnalogPotentiometer");
    m_initAnalogInput = false;

    m_fullRange = fullRange;
    m_offset = offset;

    init();
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>
   * Use the scale value so that the output produces meaningful values. I.E: you
   * have a 270
   * degree potentiometer, and you want the output to be degrees with the starting
   * point as 0
   * degrees. The scale value is 270.0(degrees).
   *
   * @param channel The analog input channel this potentiometer is plugged into.
   *                0-3 are on-board
   *                and 4-7 are on the MXP port.
   * @param scale   The scaling to multiply the voltage by to get a meaningful
   *                unit.
   */
  public AnalogPotentiometer(final int channel, double scale) {
    this(channel, scale, 0);
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>
   * Use the fullRange and offset values so that the output produces meaningful
   * values. I.E: you
   * have a 270 degree potentiometer, and you want the output to be degrees with
   * the starting point
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
   * <p>
   * The potentiometer will return a value between 0 and 1.0.
   *
   * @param channel The analog input channel this potentiometer is plugged into.
   *                0-3 are on-board
   *                and 4-7 are on the MXP port.
   */
  public AnalogPotentiometer(final int channel) {
    this(channel, 1, 0);
  }

  /**
   * AnalogPotentiometer constructor.
   *
   * <p>
   * The potentiometer will return a value between 0 and 1.0.
   *
   * @param input The {@link AnalogInput} this potentiometer is plugged into.
   */
  public AnalogPotentiometer(final AnalogInput input) {
    this(input, 1, 0);
  }

  private boolean doubleEquals(double a, double b) {
    double epsilon = 0.00001d;
    return Math.abs(a - b) < epsilon;
  }

  private double getRollovers() {
    if (m_simPosition != null) {
      return m_simPosition.get();
    }

    // As the values are not atomic, keep trying until we get 2 reads of the same
    // value. If we don't within 10 attempts, warn
    for (int i = 0; i < 10; i++) {
      double counter = m_rotationCounter.get();
      double pos = m_analogInput.getVoltage();
      double counter2 = m_rotationCounter.get();
      double pos2 = m_analogInput.getVoltage();
      if (counter == counter2 && doubleEquals(pos, pos2)) {
        pos = pos / RobotController.getVoltage5V();
        double position = (counter + pos) * m_fullRange + m_offset;
        m_lastPosition = position;
        return position;
      }
    }

    DriverStation.reportWarning(
        "Failed to read Analog Encoder. Potential Speed Overrun. Returning last value", false);
    return m_lastPosition;
  }

  /**
   * Get the current reading of the potentiometer.
   *
   * @return The current position of the potentiometer.
   */
  public double get() {
    if (m_simPosition != null) {
      return m_simPosition.get();
    }

    if (m_rotationCounter != null) {
      return getRollovers();
    }

    return (m_analogInput.getAverageVoltage() / RobotController.getVoltage5V()) * m_fullRange
        + m_offset;
  }

  public void enableRolloverSupport() {
    if (m_rotationCounter != null) {
      return;
    }
    m_rotationTrigger = new AnalogTrigger(m_analogInput);
    m_rotationCounter = new Counter();

    // Limits need to be 25% from each end
    m_rotationTrigger.setLimitsVoltage(1.25, 3.75);
    m_rotationCounter.setUpSource(m_rotationTrigger, AnalogTriggerType.kRisingPulse);
    m_rotationCounter.setDownSource(m_rotationTrigger, AnalogTriggerType.kFallingPulse);
  }

  public void resetRollovers() {
    if (m_rotationCounter != null) {
      m_rotationCounter.reset();
    }
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
    if (m_rotationCounter != null) {
      m_rotationCounter.close();
      m_rotationTrigger.close();
      m_rotationCounter = null;
      m_rotationTrigger = null;
    }
    if (m_initAnalogInput) {
      m_analogInput.close();
      m_analogInput = null;
      m_initAnalogInput = false;
    }

  }
}
