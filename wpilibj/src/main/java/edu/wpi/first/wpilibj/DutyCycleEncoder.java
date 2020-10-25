/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.AnalogTriggerOutput.AnalogTriggerType;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * Class for supporting duty cycle/PWM encoders, such as the US Digital MA3 with
 * PWM Output, the CTRE Mag Encoder, the Rev Hex Encoder, and the AM Mag
 * Encoder.
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

  protected SimDevice m_simDevice;
  protected SimDouble m_simPosition;
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
    m_simDevice = SimDevice.create("DutyCycleEncoder", m_dutyCycle.getFPGAIndex());

    if (m_simDevice != null) {
      m_simPosition = m_simDevice.createDouble("Position", false, 0.0);
      m_simDistancePerRotation = m_simDevice.createDouble("DistancePerRotation", false, 1.0);
      m_simIsConnected = m_simDevice.createBoolean("Connected", false, true);
    } else {
      m_counter = new Counter();
      m_analogTrigger = new AnalogTrigger(m_dutyCycle);
      m_analogTrigger.setLimitsDutyCycle(0.25, 0.75);
      m_counter.setUpSource(m_analogTrigger, AnalogTriggerType.kRisingPulse);
      m_counter.setDownSource(m_analogTrigger, AnalogTriggerType.kFallingPulse);
    }

    SendableRegistry.addLW(this, "DutyCycle Encoder", m_dutyCycle.getSourceChannel());
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
      if (counter == counter2 && pos == pos2) {
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
   * Get the offset of position relative to the last reset.
   *
   * <p>getPositionInRotation() - getPositionOffset() will give an encoder absolute
   * position relative to the last reset. This could potentially be negative,
   * which needs to be accounted for.
   *
   * @return the position offset
   */
  public double getPositionOffset() {
    return m_positionOffset;
  }

  /**
   * Set the distance per rotation of the encoder. This sets the multiplier used
   * to determine the distance driven based on the rotation value from the
   * encoder. Set this value based on the how far the mechanism travels in 1
   * rotation of the encoder, and factor in gearing reductions following the
   * encoder shaft. This distance can be in any units you like, linear or angular.
   *
   * @param distancePerRotation the distance per rotation of the encoder
   */
  public void setDistancePerRotation(double distancePerRotation) {
    m_distancePerRotation = distancePerRotation;

    if (m_simDistancePerRotation != null) {
      m_simDistancePerRotation.set(distancePerRotation);
    }
  }

  /**
   * Get the distance per rotation for this encoder.
   *
   * @return The scale factor that will be used to convert rotation to useful
   *         units.
   */
  public double getDistancePerRotation() {
    return m_distancePerRotation;
  }

  /**
   * Get the distance the sensor has driven since the last reset as scaled by the
   * value from {@link #setDistancePerRotation(double)}.
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

  /**
   * Reset the Encoder distance to zero.
   */
  public void reset() {
    if (m_counter != null) {
      m_counter.reset();
    }
    m_positionOffset = m_dutyCycle.getOutput();
  }

  /**
   * Get if the sensor is connected
   *
   * <p>This uses the duty cycle frequency to determine if the sensor is connected.
   * By default, a value of 100 Hz is used as the threshold, and this value can be
   * changed with {@link #setConnectedFrequencyThreshold(int)}.
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
   * Change the frequency threshold for detecting connection used by
   * {@link #isConnected()}.
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
  @SuppressWarnings("AbbreviationAsWordInName")
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
