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
import edu.wpi.first.wpilibj.AnalogTriggerOutput.AnalogTriggerType;

/**
 * Class for supporting duty cycle/PWM encoders, such as the US Digital MA3 with PWM Output, the
 * CTRE Mag Encoder, the Rev Hex Encoder, and the AM Mag Encoder.
 */
public class DutyCycleEncoder implements Sendable, AutoCloseable {
  private final DutyCycle m_dutyCycle;
  private boolean m_ownsDutyCycle;
  private DigitalInput m_digitalInput;
  private AnalogTrigger m_analogTrigger;
  private Counter m_counter;
  private int m_frequencyThreshold = 100;
  private double m_positionOffset;
  private double m_distancePerRotation = 1.0;
  private double m_lastPosition;
  private double m_sensorMin;
  private double m_sensorMax = 1.0;

  protected SimDevice m_simDevice;
  protected SimDouble m_simPosition;
  protected SimDouble m_simAbsolutePosition;
  protected SimDouble m_simDistancePerRotation;
  protected SimBoolean m_simIsConnected;

  /**
   * Construct a new DutyCycleEncoder on a specific channel.
   *
   * @param channel the channel to attach to
   */
  public DutyCycleEncoder(int channel) {
    m_digitalInput = new DigitalInput(channel);
    m_ownsDutyCycle = true;
    m_dutyCycle = new DutyCycle(m_digitalInput);
    init();
  }

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * @param dutyCycle the duty cycle to attach to
   */
  public DutyCycleEncoder(DutyCycle dutyCycle) {
    m_dutyCycle = dutyCycle;
    init();
  }

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * @param source the digital source to attach to
   */
  public DutyCycleEncoder(DigitalSource source) {
    m_ownsDutyCycle = true;
    m_dutyCycle = new DutyCycle(source);
    init();
  }

  private void init() {
    m_simDevice = SimDevice.create("DutyCycle:DutyCycleEncoder", m_dutyCycle.getSourceChannel());

    if (m_simDevice != null) {
      m_simPosition = m_simDevice.createDouble("position", SimDevice.Direction.kInput, 0.0);
      m_simDistancePerRotation =
          m_simDevice.createDouble("distance_per_rot", SimDevice.Direction.kOutput, 1.0);
      m_simAbsolutePosition =
          m_simDevice.createDouble("absPosition", SimDevice.Direction.kInput, 0.0);
      m_simIsConnected = m_simDevice.createBoolean("connected", SimDevice.Direction.kInput, true);
    } else {
      m_counter = new Counter();
      m_analogTrigger = new AnalogTrigger(m_dutyCycle);
      m_analogTrigger.setLimitsDutyCycle(0.25, 0.75);
      m_counter.setUpSource(m_analogTrigger, AnalogTriggerType.kRisingPulse);
      m_counter.setDownSource(m_analogTrigger, AnalogTriggerType.kFallingPulse);
    }

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

  private boolean doubleEquals(double a, double b) {
    double epsilon = 0.00001d;
    return Math.abs(a - b) < epsilon;
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

    // As the values are not atomic, keep trying until we get 2 reads of the same
    // value
    // If we don't within 10 attempts, error
    for (int i = 0; i < 10; i++) {
      double counter = m_counter.get();
      double pos = m_dutyCycle.getOutput();
      double counter2 = m_counter.get();
      double pos2 = m_dutyCycle.getOutput();
      if (counter == counter2 && doubleEquals(pos, pos2)) {
        // map sensor range
        pos = mapSensorRange(pos);
        double position = counter + pos - m_positionOffset;
        m_lastPosition = position;
        return position;
      }
    }

    DriverStation.reportWarning(
        "Failed to read Analog Encoder. Potential Speed Overrun. Returning last value", false);
    return m_lastPosition;
  }

  /**
   * Get the absolute position of the duty cycle encoder.
   *
   * <p>getAbsolutePosition() - getPositionOffset() will give an encoder absolute position relative
   * to the last reset. This could potentially be negative, which needs to be accounted for.
   *
   * <p>This will not account for rollovers, and will always be just the raw absolute position.
   *
   * @return the absolute position
   */
  public double getAbsolutePosition() {
    if (m_simAbsolutePosition != null) {
      return m_simAbsolutePosition.get();
    }

    return mapSensorRange(m_dutyCycle.getOutput());
  }

  /**
   * Get the offset of position relative to the last reset.
   *
   * <p>getAbsolutePosition() - getPositionOffset() will give an encoder absolute position relative
   * to the last reset. This could potentially be negative, which needs to be accounted for.
   *
   * @return the position offset
   */
  public double getPositionOffset() {
    return m_positionOffset;
  }

  /**
   * Set the position offset.
   *
   * <p>This must be in the range of 0-1.
   *
   * @param offset the offset
   */
  public void setPositionOffset(double offset) {
    m_positionOffset = MathUtil.clamp(offset, 0.0, 1.0);
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
   * Set the distance per rotation of the encoder. This sets the multiplier used to determine the
   * distance driven based on the rotation value from the encoder. Set this value based on the how
   * far the mechanism travels in 1 rotation of the encoder, and factor in gearing reductions
   * following the encoder shaft. This distance can be in any units you like, linear or angular.
   *
   * @param distancePerRotation the distance per rotation of the encoder
   */
  public void setDistancePerRotation(double distancePerRotation) {
    m_distancePerRotation = distancePerRotation;
  }

  /**
   * Get the distance per rotation for this encoder.
   *
   * @return The scale factor that will be used to convert rotation to useful units.
   */
  public double getDistancePerRotation() {
    return m_distancePerRotation;
  }

  /**
   * Get the distance the sensor has driven since the last reset as scaled by the value from {@link
   * #setDistancePerRotation(double)}.
   *
   * @return The distance driven since the last reset
   */
  public double getDistance() {
    return get() * getDistancePerRotation();
  }

  /**
   * Get the frequency in Hz of the duty cycle signal from the encoder.
   *
   * @return duty cycle frequency in Hz
   */
  public int getFrequency() {
    return m_dutyCycle.getFrequency();
  }

  /** Reset the Encoder distance to zero. */
  public void reset() {
    if (m_counter != null) {
      m_counter.reset();
    }
    m_positionOffset = m_dutyCycle.getOutput();
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

  @Override
  public void close() {
    if (m_counter != null) {
      m_counter.close();
    }
    if (m_analogTrigger != null) {
      m_analogTrigger.close();
    }
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
    builder.addDoubleProperty("Distance", this::getDistance, null);
    builder.addDoubleProperty("Distance Per Rotation", this::getDistancePerRotation, null);
    builder.addBooleanProperty("Is Connected", this::isConnected, null);
  }
}
