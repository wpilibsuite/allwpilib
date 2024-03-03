// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Class for supporting duty cycle/PWM encoders, such as the US Digital MA3 with PWM Output, the
 * CTRE Mag Encoder, the Rev Hex Encoder, and the AM Mag Encoder.
 */
public class DutyCycleEncoder implements Sendable, AutoCloseable {
  private final DutyCycle m_dutyCycle;
  private boolean m_ownsDutyCycle;
  private DigitalInput m_digitalInput;
  private int m_frequencyThreshold = 100;
  private double m_fullRange;
  private double m_expectedZero;
  private double m_periodNanos;
  private double m_sensorMin;
  private double m_sensorMax = 1.0;
  private boolean m_isInverted;

  private SimDevice m_simDevice;
  private SimDouble m_simPosition;
  private SimBoolean m_simIsConnected;

  /**
   * Construct a new DutyCycleEncoder on a specific channel.
   *
   * @param channel the channel to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  @SuppressWarnings("this-escape")
  public DutyCycleEncoder(int channel, double fullRange, double expectedZero) {
    m_digitalInput = new DigitalInput(channel);
    m_ownsDutyCycle = true;
    m_dutyCycle = new DutyCycle(m_digitalInput);
    init(fullRange, expectedZero);
  }

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * @param dutyCycle the duty cycle to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  @SuppressWarnings("this-escape")
  public DutyCycleEncoder(DutyCycle dutyCycle, double fullRange, double expectedZero) {
    m_dutyCycle = dutyCycle;
    init(fullRange, expectedZero);
  }

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * @param source the digital source to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  @SuppressWarnings("this-escape")
  public DutyCycleEncoder(DigitalSource source, double fullRange, double expectedZero) {
    m_ownsDutyCycle = true;
    m_dutyCycle = new DutyCycle(source);
    init(fullRange, expectedZero);
  }

  /**
   * Construct a new DutyCycleEncoder on a specific channel.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param channel the channel to attach to
   */
  @SuppressWarnings("this-escape")
  public DutyCycleEncoder(int channel) {
    this(channel, 1.0, 0.0);
  }

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param dutyCycle the duty cycle to attach to
   */
  @SuppressWarnings("this-escape")
  public DutyCycleEncoder(DutyCycle dutyCycle) {
    this(dutyCycle, 1.0, 0.0);
  }

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param source the digital source to attach to
   */
  @SuppressWarnings("this-escape")
  public DutyCycleEncoder(DigitalSource source) {
    this(source, 1.0, 0.0);
  }

  private void init(double fullRange, double expectedZero) {
    m_simDevice = SimDevice.create("DutyCycle:DutyCycleEncoder", m_dutyCycle.getSourceChannel());

    if (m_simDevice != null) {
      m_simPosition = m_simDevice.createDouble("Position", SimDevice.Direction.kInput, 0.0);
      m_simIsConnected = m_simDevice.createBoolean("Connected", SimDevice.Direction.kInput, true);
    }

    m_fullRange = fullRange;
    m_expectedZero = expectedZero;

    SendableRegistry.addLW(this, "DutyCycle Encoder", m_dutyCycle.getSourceChannel());
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
   * <p>This is reported in rotations since the last reset.
   *
   * @return the encoder value in rotations
   */
  public double get() {
    if (m_simPosition != null) {
      return m_simPosition.get();
    }

    double pos;
    // Compute output percentage (0-1)
    if (m_periodNanos == 0.0) {
      pos = m_dutyCycle.getOutput();
    } else {
      int highTime = m_dutyCycle.getHighTimeNanoseconds();
      pos = highTime / m_periodNanos;
    }

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
   * Set the encoder duty cycle range. As the encoder needs to maintain a duty cycle, the duty cycle
   * cannot go all the way to 0% or all the way to 100%. For example, an encoder with a 4096 us
   * period might have a minimum duty cycle of 1 us / 4096 us and a maximum duty cycle of 4095 /
   * 4096 us. Setting the range will result in an encoder duty cycle less than or equal to the
   * minimum being output as 0 rotation, the duty cycle greater than or equal to the maximum being
   * output as 1 rotation, and values in between linearly scaled from 0 to 1.
   *
   * @param min minimum duty cycle (0-1 range)
   * @param max maximum duty cycle (0-1 range)
   */
  public void setDutyCycleRange(double min, double max) {
    m_sensorMin = MathUtil.clamp(min, 0.0, 1.0);
    m_sensorMax = MathUtil.clamp(max, 0.0, 1.0);
  }

  /**
   * Get the frequency in Hz of the duty cycle signal from the encoder.
   *
   * @return duty cycle frequency in Hz
   */
  public int getFrequency() {
    return m_dutyCycle.getFrequency();
  }

  /**
   * Get if the sensor is connected
   *
   * <p>This uses the duty cycle frequency to determine if the sensor is connected. By default, a
   * value of 100 Hz is used as the threshold, and this value can be changed with {@link
   * #setConnectedFrequencyThreshold(int)}.
   *
   * @return true if the sensor is connected
   */
  public boolean isConnected() {
    if (m_simIsConnected != null) {
      return m_simIsConnected.get();
    }
    return getFrequency() > m_frequencyThreshold;
  }

  /**
   * Change the frequency threshold for detecting connection used by {@link #isConnected()}.
   *
   * @param frequency the minimum frequency in Hz.
   */
  public void setConnectedFrequencyThreshold(int frequency) {
    if (frequency < 0) {
      frequency = 0;
    }

    m_frequencyThreshold = frequency;
  }

  /**
   * Sets the assumed frequency of the connected device.
   *
   * <p>By default, the DutyCycle engine has to compute the frequency of the input signal. This can
   * result in both delayed readings and jumpy readings. To solve this, you can pass the expected
   * frequency of the sensor to this function. This will use that frequency to compute the DutyCycle
   * percentage, rather than the computed frequency.
   *
   * @param frequency the assumed frequency of the sensor
   */
  public void setAssumedFrequency(double frequency) {
    if (frequency == 0.0) {
      m_periodNanos = 0.0;
    } else {
      m_periodNanos = 1000000000 / frequency;
    }
  }

  /**
   * Set if this encoder is inverted.
   *
   * @param inverted true to invert the encoder, false otherwise
   */
  public void setInverted(boolean inverted) {
    m_isInverted = inverted;
  }

  @Override
  public void close() {
    if (m_ownsDutyCycle) {
      m_dutyCycle.close();
    }
    if (m_digitalInput != null) {
      m_digitalInput.close();
    }
    if (m_simDevice != null) {
      m_simDevice.close();
    }
  }

  /**
   * Get the FPGA index for the DutyCycleEncoder.
   *
   * @return the FPGA index
   */
  public int getFPGAIndex() {
    return m_dutyCycle.getFPGAIndex();
  }

  /**
   * Get the channel of the source.
   *
   * @return the source channel
   */
  public int getSourceChannel() {
    return m_dutyCycle.getSourceChannel();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("AbsoluteEncoder");
    builder.addDoubleProperty("Position", this::get, null);
    builder.addBooleanProperty("Is Connected", this::isConnected, null);
  }
}
