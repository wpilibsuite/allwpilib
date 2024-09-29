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

/** Class for supporting continuous analog encoders, such as the US Digital MA3. */
public class AnalogEncoder implements Sendable, AutoCloseable {
  private final AnalogInput m_analogInput;
  private boolean m_ownsAnalogInput;
  private double m_fullRange;
  private double m_expectedZero;
  private double m_sensorMin;
  private double m_sensorMax = 1.0;
  private boolean m_isInverted;

  private SimDevice m_simDevice;
  private SimDouble m_simPosition;

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogIn channel.
   *
   * @param channel the analog input channel to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  public AnalogEncoder(int channel, double fullRange, double expectedZero) {
    this(new AnalogInput(channel), fullRange, expectedZero);
    m_ownsAnalogInput = true;
  }

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  @SuppressWarnings("this-escape")
  public AnalogEncoder(AnalogInput analogInput, double fullRange, double expectedZero) {
    m_analogInput = analogInput;
    init(fullRange, expectedZero);
  }

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogIn channel.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param channel the analog input channel to attach to
   */
  public AnalogEncoder(int channel) {
    this(channel, 1.0, 0.0);
  }

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
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
   * Get the encoder value.
   *
   * @return the encoder value scaled by the full range input
   */
  public double get() {
    if (m_simPosition != null) {
      return m_simPosition.get();
    }

    double analog = m_analogInput.getVoltage();
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

  /**
   * Set the encoder voltage percentage range. Analog sensors are not always fully stable at the end
   * of their travel ranges. Shrinking this range down can help mitigate issues with that.
   *
   * @param min minimum voltage percentage (0-1 range)
   * @param max maximum voltage percentage (0-1 range)
   */
  public void setVoltagePercentageRange(double min, double max) {
    m_sensorMin = MathUtil.clamp(min, 0.0, 1.0);
    m_sensorMax = MathUtil.clamp(max, 0.0, 1.0);
  }

  /**
   * Set if this encoder is inverted.
   *
   * @param inverted true to invert the encoder, false otherwise
   */
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

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("AbsoluteEncoder");
    builder.addDoubleProperty("Position", this::get, null);
  }
}
