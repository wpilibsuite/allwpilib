// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDevice.Direction;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.AnalogTriggerOutput.AnalogTriggerType;

/**
 * Class for supporting continuous analog encoders, such as the US Digital MA3.
 */
public class AnalogEncoder implements Sendable, AutoCloseable {
  private final AnalogInput m_analogInput;
  private boolean m_ownsAnalogInput;
  private double m_fullRange;
  private double m_expectedZero;
  private double m_sensorMin;
  private double m_sensorMax = 1.0;
  private boolean m_isInverted;

  private AnalogTrigger m_rolloverTrigger;
  private Counter m_rolloverCounter;
  private double m_lastPosition;

  private SimDevice m_simDevice;
  private SimDouble m_simPosition;

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogIn channel.
   *
   * @param channel the analog input channel to attach to
   */
  public AnalogEncoder(int channel, double fullRange, double expectedZero) {
    this(new AnalogInput(channel), fullRange, expectedZero);
    m_ownsAnalogInput = true;
  }

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  @SuppressWarnings("this-escape")
  public AnalogEncoder(AnalogInput analogInput, double fullRange, double expectedZero) {
    m_analogInput = analogInput;
    init(fullRange, expectedZero);
  }

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogIn channel.
   *
   * @param channel the analog input channel to attach to
   */
  public AnalogEncoder(int channel) {
    this(channel, 1.0, 0.0);
  }

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  @SuppressWarnings("this-escape")
  public AnalogEncoder(AnalogInput analogInput) {
    this(analogInput, 1.0, 0.0);
  }

  private void init(double fullRange, double expectedZero) {
    m_simDevice = SimDevice.create("AnalogEncoder", m_analogInput.getChannel());

    if (m_simDevice != null) {
      m_simPosition = m_simDevice.createDouble("Position", Direction.kInput, 0.0);
    }

    m_fullRange = fullRange;
    m_expectedZero = expectedZero;

    SendableRegistry.addLW(this, "Analog Encoder", m_analogInput.getChannel());
  }

  private double mapSensorRange(double pos) {
    // map sensor range
    if (pos < m_sensorMin) {
      pos = m_sensorMin;
    }
    if (pos > m_sensorMax) {
      pos = m_sensorMax;
    }
    pos = (pos - m_sensorMin) / (m_sensorMax - m_sensorMin);
    return pos;
  }

  /**
   * Get the encoder value since the last reset.
   *
   * <p>
   * This is reported in rotations since the last reset.
   *
   * @return the encoder value in rotations
   */
  public double get() {
    if (m_simPosition != null) {
      return m_simPosition.get();
    }

    if (m_rolloverCounter != null) {
      return getWithRollovers();
    } else {
      double analog = m_analogInput.getVoltage();
      return getWithoutRollovers(analog);
    }
  }

  /**
   * Set the encoder voltage percentage range. Analog sensors are not
   * always fully stable at the end of their travel ranges. Shrinking
   * this range down can help mitigate issues with that.
   *
   * @param min minimum voltage percentage (0-1 range)
   * @param max maximum voltage percentage (0-1 range)
   */
  public void setVoltagePercentageRange(double min, double max) {
    m_sensorMin = MathUtil.clamp(min, 0.0, 1.0);
    m_sensorMax = MathUtil.clamp(max, 0.0, 1.0);
  }

  public void setInverted(boolean inverted) {
    m_isInverted = inverted;
  }

  /**
   * Get the channel number.
   *
   * @return The channel number.
   */
  public int getChannel() {
    return m_analogInput.getChannel();
  }

  @Override
  public void close() {
    if (m_ownsAnalogInput) {
      m_analogInput.close();
    }
    if (m_simDevice != null) {
      m_simDevice.close();
    }
  }

  public void configureRolloverSupport(boolean enable) {
    if (enable && m_rolloverCounter == null) {
      m_rolloverTrigger = new AnalogTrigger(m_analogInput);
      m_rolloverTrigger.setLimitsVoltage(1.25, 3.75);
      m_rolloverCounter = new Counter();
      m_rolloverCounter.setUpSource(m_rolloverTrigger.createOutput(AnalogTriggerType.kRisingPulse));
      m_rolloverCounter.setDownSource(m_rolloverTrigger.createOutput(AnalogTriggerType.kFallingPulse));
    } else if (!enable && m_rolloverCounter != null) {
      m_rolloverTrigger.close();
      m_rolloverTrigger = null;
      m_rolloverCounter.close();
      m_rolloverCounter = null;
    }
  }

  public void resetRollovers() {
    if (m_rolloverCounter != null) {
      m_rolloverCounter.reset();
    }
  }

  private boolean doubleEquals(double a, double b) {
    double epsilon = 0.00001d;
    return Math.abs(a - b) < epsilon;
  }

  private double getWithRollovers() {
    // As the values are not atomic, keep trying until we get 2 reads of the same
    // value. If we don't within 10 attempts, warn
    for (int i = 0; i < 10; i++) {
      double counter = m_rolloverCounter.get();
      double pos = m_analogInput.getVoltage();
      double counter2 = m_rolloverCounter.get();
      double pos2 = m_analogInput.getVoltage();
      if (counter == counter2 && doubleEquals(pos, pos2)) {
        pos = getWithoutRollovers(pos);

        if (m_isInverted) {
          pos = pos - counter;
        } else {
          pos = pos + counter;
        }

        m_lastPosition = pos;
        return pos;
      }
    }

    DriverStation.reportWarning(
        "Failed to read Analog Encoder. Potential Speed Overrun. Returning last value", false);
    return m_lastPosition;
  }

  private double getWithoutRollovers(double analog) {
    double pos = analog / RobotController.getVoltage5V();

    // Map sensor range if range isn't full
    pos = mapSensorRange(pos);

    // Compute full range and offset
    pos = pos * m_fullRange - m_expectedZero;

    // Map from 0 - Full Range
    double result = MathUtil.inputModulus(pos, 0, m_fullRange);
    // Invert if necessary
    if (m_isInverted) {
      return m_fullRange - result;
    }
    return result;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("AbsoluteEncoder");
    builder.addDoubleProperty("Position", this::get, null);
  }
}
