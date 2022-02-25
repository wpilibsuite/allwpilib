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

/** Class for supporting continuous analog encoders, such as the US Digital MA3. */
public class AnalogEncoder implements Sendable, AutoCloseable {
  private final AnalogInput m_analogInput;
  private AnalogTrigger m_analogTrigger;
  private Counter m_counter;
  private double m_positionOffset;
  private double m_distancePerRotation = 1.0;
  private double m_lastPosition;

  protected SimDevice m_simDevice;
  protected SimDouble m_simPosition;
  protected SimDouble m_simAbsolutePosition;

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogIn channel.
   *
   * @param channel the analog input channel to attach to
   */
  public AnalogEncoder(int channel) {
    this(new AnalogInput(channel));
  }

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  public AnalogEncoder(AnalogInput analogInput) {
    m_analogInput = analogInput;
    init();
  }

  private void init() {
    m_analogTrigger = new AnalogTrigger(m_analogInput);
    m_counter = new Counter();

    m_simDevice = SimDevice.create("AnalogEncoder", m_analogInput.getChannel());

    if (m_simDevice != null) {
      m_simPosition = m_simDevice.createDouble("Position", Direction.kInput, 0.0);
      m_simAbsolutePosition = m_simDevice.createDouble("absPosition", Direction.kInput, 0.0);
    }

    // Limits need to be 25% from each end
    m_analogTrigger.setLimitsVoltage(1.25, 3.75);
    m_counter.setUpSource(m_analogTrigger, AnalogTriggerType.kRisingPulse);
    m_counter.setDownSource(m_analogTrigger, AnalogTriggerType.kFallingPulse);

    SendableRegistry.addLW(this, "Analog Encoder", m_analogInput.getChannel());
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
    // value. If we don't within 10 attempts, warn
    for (int i = 0; i < 10; i++) {
      double counter = m_counter.get();
      double pos = m_analogInput.getVoltage();
      double counter2 = m_counter.get();
      double pos2 = m_analogInput.getVoltage();
      if (counter == counter2 && doubleEquals(pos, pos2)) {
        pos = pos / RobotController.getVoltage5V();
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
   * Get the absolute position of the analog encoder.
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

    return m_analogInput.getVoltage() / RobotController.getVoltage5V();
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
   * Get the channel number.
   *
   * @return The channel number.
   */
  public int getChannel() {
    return m_analogInput.getChannel();
  }

  /** Reset the Encoder distance to zero. */
  public void reset() {
    m_counter.reset();
    m_positionOffset = m_analogInput.getVoltage() / RobotController.getVoltage5V();
  }

  @Override
  public void close() {
    m_counter.close();
    m_analogTrigger.close();
    if (m_simDevice != null) {
      m_simDevice.close();
    }
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("AbsoluteEncoder");
    builder.addDoubleProperty("Distance", this::getDistance, null);
    builder.addDoubleProperty("Distance Per Rotation", this::getDistancePerRotation, null);
  }
}
