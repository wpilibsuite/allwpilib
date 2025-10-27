// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.EncoderJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Class to read quadrature encoders.
 *
 * <p>Quadrature encoders are devices that count shaft rotation and can sense direction. The output
 * of the Encoder class is an integer that can count either up or down, and can go negative for
 * reverse direction counting. When creating Encoders, a direction can be supplied that inverts the
 * sense of the output to make code more readable if the encoder is mounted such that forward
 * movement generates negative values. Quadrature encoders have two digital outputs, an A Channel
 * and a B Channel, that are out of phase with each other for direction sensing.
 *
 * <p>All encoders will immediately start counting - reset() them if you need them to be zeroed
 * before use.
 */
public class Encoder implements CounterBase, Sendable, AutoCloseable {
  private final EncodingType m_encodingType;

  int m_encoder; // the HAL encoder object

  /**
   * Common initialization code for Encoders. This code allocates resources for Encoders and is
   * common to all constructors.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param aChannel The a channel.
   * @param bChannel The b channel.
   * @param reverseDirection If true, counts down instead of up (this is all relative)
   */
  private void initEncoder(
      int aChannel, int bChannel, boolean reverseDirection, final EncodingType type) {
    m_encoder = EncoderJNI.initializeEncoder(aChannel, bChannel, reverseDirection, type.value);

    String typeStr =
        switch (type) {
          case k1X -> "Encoder:1x";
          case k2X -> "Encoder:2x";
          case k4X -> "Encoder:4x";
          default -> "Encoder";
        };
    HAL.reportUsage("IO[" + aChannel + "," + bChannel + "]", typeStr);

    int fpgaIndex = getFPGAIndex();
    SendableRegistry.add(this, "Encoder", fpgaIndex);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA The 'a' channel DIO channel. 0-9 are on-board, 10-25 are on the MXP port
   * @param channelB The 'b' channel DIO channel. 0-9 are on-board, 10-25 are on the MXP port
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *     if necessary so forward represents positive values.
   */
  public Encoder(final int channelA, final int channelB, boolean reverseDirection) {
    this(channelA, channelB, reverseDirection, EncodingType.k4X);
  }

  /**
   * Encoder constructor. Construct an Encoder given a and b channels.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA The a channel digital input channel.
   * @param channelB The b channel digital input channel.
   */
  public Encoder(final int channelA, final int channelB) {
    this(channelA, channelB, false);
  }

  /**
   * Encoder constructor. Construct an Encoder given a and b channels.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA The a channel digital input channel.
   * @param channelB The b channel digital input channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *     if necessary so forward represents positive values.
   * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
   *     selected, then an encoder FPGA object is used and the returned counts will be 4x the
   *     encoder spec'd value since all rising and falling edges are counted. If 1X or 2X are
   *     selected, then a counter object will be used and the returned value will either exactly
   *     match the spec'd count or be double (2x) the spec'd count.
   */
  @SuppressWarnings("this-escape")
  public Encoder(
      final int channelA,
      final int channelB,
      boolean reverseDirection,
      final EncodingType encodingType) {
    requireNonNullParam(encodingType, "encodingType", "Encoder");

    m_encodingType = encodingType;
    // SendableRegistry.addChild(this, m_aSource);
    // SendableRegistry.addChild(this, m_bSource);
    initEncoder(channelA, channelB, reverseDirection, encodingType);
  }

  /**
   * Get the FPGA index of the encoder.
   *
   * @return The Encoder's FPGA index.
   */
  public int getFPGAIndex() {
    return EncoderJNI.getEncoderFPGAIndex(m_encoder);
  }

  /**
   * Used to divide raw edge counts down to spec'd counts.
   *
   * @return The encoding scale factor 1x, 2x, or 4x, per the requested encoding type.
   */
  public int getEncodingScale() {
    return EncoderJNI.getEncoderEncodingScale(m_encoder);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    // if (m_aSource != null && m_allocatedA) {
    //   m_aSource.close();
    //   m_allocatedA = false;
    // }
    // if (m_bSource != null && m_allocatedB) {
    //   m_bSource.close();
    //   m_allocatedB = false;
    // }
    // if (m_indexSource != null && m_allocatedI) {
    //   m_indexSource.close();
    //   m_allocatedI = false;
    // }

    // m_aSource = null;
    // m_bSource = null;
    // m_indexSource = null;
    EncoderJNI.freeEncoder(m_encoder);
    m_encoder = 0;
  }

  /**
   * Gets the raw value from the encoder. The raw value is the actual count unscaled by the 1x, 2x,
   * or 4x scale factor.
   *
   * @return Current raw count from the encoder
   */
  public int getRaw() {
    return EncoderJNI.getEncoderRaw(m_encoder);
  }

  /**
   * Gets the current count. Returns the current count on the Encoder. This method compensates for
   * the decoding type.
   *
   * @return Current count from the Encoder adjusted for the 1x, 2x, or 4x scale factor.
   */
  @Override
  public int get() {
    return EncoderJNI.getEncoder(m_encoder);
  }

  /** Reset the Encoder distance to zero. Resets the current count to zero on the encoder. */
  @Override
  public void reset() {
    EncoderJNI.resetEncoder(m_encoder);
  }

  /**
   * Returns the period of the most recent pulse. Returns the period of the most recent Encoder
   * pulse in seconds. This method compensates for the decoding type.
   *
   * <p><b>Warning:</b> This returns unscaled periods. Use getRate() for rates that are scaled using
   * the value from setDistancePerPulse().
   *
   * @return Period in seconds of the most recent pulse.
   * @deprecated Use getRate() in favor of this method.
   */
  @Override
  @Deprecated
  public double getPeriod() {
    return EncoderJNI.getEncoderPeriod(m_encoder);
  }

  /**
   * Sets the maximum period for stopped detection. Sets the value that represents the maximum
   * period of the Encoder before it will assume that the attached device is stopped. This timeout
   * allows users to determine if the wheels or other shaft has stopped rotating. This method
   * compensates for the decoding type.
   *
   * @param maxPeriod The maximum time between rising and falling edges before the FPGA will report
   *     the device stopped. This is expressed in seconds.
   * @deprecated Use setMinRate() in favor of this method. This takes unscaled periods and
   *     setMinRate() scales using value from setDistancePerPulse().
   */
  @Override
  @Deprecated
  public void setMaxPeriod(double maxPeriod) {
    EncoderJNI.setEncoderMaxPeriod(m_encoder, maxPeriod);
  }

  /**
   * Determine if the encoder is stopped. Using the MaxPeriod value, a boolean is returned that is
   * true if the encoder is considered stopped and false if it is still moving. A stopped encoder is
   * one where the most recent pulse width exceeds the MaxPeriod.
   *
   * @return True if the encoder is considered stopped.
   */
  @Override
  public boolean getStopped() {
    return EncoderJNI.getEncoderStopped(m_encoder);
  }

  /**
   * The last direction the encoder value changed.
   *
   * @return The last direction the encoder value changed.
   */
  @Override
  public boolean getDirection() {
    return EncoderJNI.getEncoderDirection(m_encoder);
  }

  /**
   * Get the distance the robot has driven since the last reset as scaled by the value from {@link
   * #setDistancePerPulse(double)}.
   *
   * @return The distance driven since the last reset
   */
  public double getDistance() {
    return EncoderJNI.getEncoderDistance(m_encoder);
  }

  /**
   * Get the current rate of the encoder. Units are distance per second as scaled by the value from
   * setDistancePerPulse().
   *
   * @return The current rate of the encoder.
   */
  public double getRate() {
    return EncoderJNI.getEncoderRate(m_encoder);
  }

  /**
   * Set the minimum rate of the device before the hardware reports it stopped.
   *
   * @param minRate The minimum rate. The units are in distance per second as scaled by the value
   *     from setDistancePerPulse().
   */
  public void setMinRate(double minRate) {
    EncoderJNI.setEncoderMinRate(m_encoder, minRate);
  }

  /**
   * Set the distance per pulse for this encoder. This sets the multiplier used to determine the
   * distance driven based on the count value from the encoder. Do not include the decoding type in
   * this scale. The library already compensates for the decoding type. Set this value based on the
   * encoder's rated Pulses per Revolution and factor in gearing reductions following the encoder
   * shaft. This distance can be in any units you like, linear or angular.
   *
   * @param distancePerPulse The scale factor that will be used to convert pulses to useful units.
   */
  public void setDistancePerPulse(double distancePerPulse) {
    EncoderJNI.setEncoderDistancePerPulse(m_encoder, distancePerPulse);
  }

  /**
   * Get the distance per pulse for this encoder.
   *
   * @return The scale factor that will be used to convert pulses to useful units.
   */
  public double getDistancePerPulse() {
    return EncoderJNI.getEncoderDistancePerPulse(m_encoder);
  }

  /**
   * Set the direction sensing for this encoder. This sets the direction sensing on the encoder so
   * that it could count in the correct software direction regardless of the mounting.
   *
   * @param reverseDirection true if the encoder direction should be reversed
   */
  public void setReverseDirection(boolean reverseDirection) {
    EncoderJNI.setEncoderReverseDirection(m_encoder, reverseDirection);
  }

  /**
   * Set the Samples to Average which specifies the number of samples of the timer to average when
   * calculating the period. Perform averaging to account for mechanical imperfections or as
   * oversampling to increase resolution.
   *
   * @param samplesToAverage The number of samples to average from 1 to 127.
   */
  public void setSamplesToAverage(int samplesToAverage) {
    EncoderJNI.setEncoderSamplesToAverage(m_encoder, samplesToAverage);
  }

  /**
   * Get the Samples to Average which specifies the number of samples of the timer to average when
   * calculating the period. Perform averaging to account for mechanical imperfections or as
   * oversampling to increase resolution.
   *
   * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
   */
  public int getSamplesToAverage() {
    return EncoderJNI.getEncoderSamplesToAverage(m_encoder);
  }

  /**
   * Indicates this input is used by a simulated device.
   *
   * @param device simulated device handle
   */
  public void setSimDevice(SimDevice device) {
    EncoderJNI.setEncoderSimDevice(m_encoder, device.getNativeHandle());
  }

  /**
   * Gets the decoding scale factor for scaling raw values to full counts.
   *
   * @return decoding scale factor
   */
  public double getDecodingScaleFactor() {
    return switch (m_encodingType) {
      case k1X -> 1.0;
      case k2X -> 0.5;
      case k4X -> 0.25;
    };
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    if (EncoderJNI.getEncoderEncodingType(m_encoder) == EncodingType.k4X.value) {
      builder.setSmartDashboardType("Quadrature Encoder");
    } else {
      builder.setSmartDashboardType("Encoder");
    }

    builder.addDoubleProperty("Speed", this::getRate, null);
    builder.addDoubleProperty("Distance", this::getDistance, null);
    builder.addDoubleProperty("Distance per Tick", this::getDistancePerPulse, null);
  }
}
