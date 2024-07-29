// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.AnalogGyroJNI;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Use a rate gyro to return the robots heading relative to a starting position. The Gyro class
 * tracks the robots heading based on the starting position. As the robot rotates the new heading is
 * computed by integrating the rate of rotation returned by the sensor. When the class is
 * instantiated, it does a short calibration routine where it samples the gyro while at rest to
 * determine the default offset. This is subtracted from each sample to determine the heading.
 *
 * <p>This class is for gyro sensors that connect to an analog input.
 */
public class AnalogGyro implements Sendable, AutoCloseable {
  private static final double kDefaultVoltsPerDegreePerSecond = 0.007;
  private AnalogInput m_analog;
  private boolean m_channelAllocated;

  private int m_gyroHandle;

  /** Initialize the gyro. Calibration is handled by calibrate(). */
  private void initGyro() {
    if (m_gyroHandle == 0) {
      m_gyroHandle = AnalogGyroJNI.initializeAnalogGyro(m_analog.m_port);
    }

    AnalogGyroJNI.setupAnalogGyro(m_gyroHandle);

    HAL.report(tResourceType.kResourceType_Gyro, m_analog.getChannel() + 1);
    SendableRegistry.addLW(this, "AnalogGyro", m_analog.getChannel());
  }

  /**
   * Calibrate the gyro by running for a number of samples and computing the center value. Then use
   * the center value as the Accumulator center value for subsequent measurements.
   *
   * <p>It's important to make sure that the robot is not moving while the centering calculations
   * are in progress, this is typically done when the robot is first turned on while it's sitting at
   * rest before the competition starts.
   */
  public void calibrate() {
    AnalogGyroJNI.calibrateAnalogGyro(m_gyroHandle);
  }

  /**
   * Return the heading of the robot as a {@link edu.wpi.first.math.geometry.Rotation2d}.
   *
   * <p>The angle is continuous, that is it will continue from 360 to 361 degrees. This allows
   * algorithms that wouldn't want to see a discontinuity in the gyro output as it sweeps past from
   * 360 to 0 on the second time around.
   *
   * <p>The angle is expected to increase as the gyro turns counterclockwise when looked at from the
   * top. It needs to follow the NWU axis convention.
   *
   * <p>This heading is based on integration of the returned rate from the gyro.
   *
   * @return the current heading of the robot as a {@link edu.wpi.first.math.geometry.Rotation2d}.
   */
  public Rotation2d getRotation2d() {
    return Rotation2d.fromDegrees(-getAngle());
  }

  /**
   * Gyro constructor using the channel number.
   *
   * @param channel The analog channel the gyro is connected to. Gyros can only be used on on-board
   *     channels 0-1.
   */
  @SuppressWarnings("this-escape")
  public AnalogGyro(int channel) {
    this(new AnalogInput(channel));
    m_channelAllocated = true;
    SendableRegistry.addChild(this, m_analog);
  }

  /**
   * Gyro constructor with a precreated analog channel object. Use this constructor when the analog
   * channel needs to be shared.
   *
   * @param channel The AnalogInput object that the gyro is connected to. Gyros can only be used on
   *     on-board channels 0-1.
   */
  @SuppressWarnings("this-escape")
  public AnalogGyro(AnalogInput channel) {
    requireNonNullParam(channel, "channel", "AnalogGyro");

    m_analog = channel;
    initGyro();
    calibrate();
  }

  /**
   * Gyro constructor using the channel number along with parameters for presetting the center and
   * offset values. Bypasses calibration.
   *
   * @param channel The analog channel the gyro is connected to. Gyros can only be used on on-board
   *     channels 0-1.
   * @param center Preset uncalibrated value to use as the accumulator center value.
   * @param offset Preset uncalibrated value to use as the gyro offset.
   */
  @SuppressWarnings("this-escape")
  public AnalogGyro(int channel, int center, double offset) {
    this(new AnalogInput(channel), center, offset);
    m_channelAllocated = true;
    SendableRegistry.addChild(this, m_analog);
  }

  /**
   * Gyro constructor with a precreated analog channel object along with parameters for presetting
   * the center and offset values. Bypasses calibration.
   *
   * @param channel The analog channel the gyro is connected to. Gyros can only be used on on-board
   *     channels 0-1.
   * @param center Preset uncalibrated value to use as the accumulator center value.
   * @param offset Preset uncalibrated value to use as the gyro offset.
   */
  @SuppressWarnings("this-escape")
  public AnalogGyro(AnalogInput channel, int center, double offset) {
    requireNonNullParam(channel, "channel", "AnalogGyro");

    m_analog = channel;
    initGyro();
    AnalogGyroJNI.setAnalogGyroParameters(
        m_gyroHandle, kDefaultVoltsPerDegreePerSecond,
        offset, center);
    reset();
  }

  /**
   * Reset the gyro.
   *
   * <p>Resets the gyro to a heading of zero. This can be used if there is significant drift in the
   * gyro, and it needs to be recalibrated after it has been running.
   */
  public void reset() {
    AnalogGyroJNI.resetAnalogGyro(m_gyroHandle);
  }

  /** Delete (free) the accumulator and the analog components used for the gyro. */
  @Override
  public void close() {
    SendableRegistry.remove(this);
    if (m_analog != null && m_channelAllocated) {
      m_analog.close();
    }
    m_analog = null;
    AnalogGyroJNI.freeAnalogGyro(m_gyroHandle);
  }

  /**
   * Return the heading of the robot in degrees.
   *
   * <p>The angle is continuous, that is it will continue from 360 to 361 degrees. This allows
   * algorithms that wouldn't want to see a discontinuity in the gyro output as it sweeps past from
   * 360 to 0 on the second time around.
   *
   * <p>The angle is expected to increase as the gyro turns clockwise when looked at from the top.
   * It needs to follow the NED axis convention.
   *
   * <p>This heading is based on integration of the returned rate from the gyro.
   *
   * @return the current heading of the robot in degrees.
   */
  public double getAngle() {
    if (m_analog == null) {
      return 0.0;
    } else {
      return AnalogGyroJNI.getAnalogGyroAngle(m_gyroHandle);
    }
  }

  /**
   * Return the rate of rotation of the gyro.
   *
   * <p>The rate is based on the most recent reading of the gyro analog value
   *
   * <p>The rate is expected to be positive as the gyro turns clockwise when looked at from the top.
   * It needs to follow the NED axis convention.
   *
   * @return the current rate in degrees per second
   */
  public double getRate() {
    if (m_analog == null) {
      return 0.0;
    } else {
      return AnalogGyroJNI.getAnalogGyroRate(m_gyroHandle);
    }
  }

  /**
   * Return the gyro offset value set during calibration to use as a future preset.
   *
   * @return the current offset value
   */
  public double getOffset() {
    return AnalogGyroJNI.getAnalogGyroOffset(m_gyroHandle);
  }

  /**
   * Return the gyro center value set during calibration to use as a future preset.
   *
   * @return the current center value
   */
  public int getCenter() {
    return AnalogGyroJNI.getAnalogGyroCenter(m_gyroHandle);
  }

  /**
   * Set the gyro sensitivity. This takes the number of volts/degree/second sensitivity of the gyro
   * and uses it in subsequent calculations to allow the code to work with multiple gyros. This
   * value is typically found in the gyro datasheet.
   *
   * @param voltsPerDegreePerSecond The sensitivity in Volts/degree/second.
   */
  public void setSensitivity(double voltsPerDegreePerSecond) {
    AnalogGyroJNI.setAnalogGyroVoltsPerDegreePerSecond(m_gyroHandle, voltsPerDegreePerSecond);
  }

  /**
   * Set the size of the neutral zone. Any voltage from the gyro less than this amount from the
   * center is considered stationary. Setting a deadband will decrease the amount of drift when the
   * gyro isn't rotating, but will make it less accurate.
   *
   * @param volts The size of the deadband in volts
   */
  public void setDeadband(double volts) {
    AnalogGyroJNI.setAnalogGyroDeadband(m_gyroHandle, volts);
  }

  /**
   * Gets the analog input for the gyro.
   *
   * @return AnalogInput
   */
  public AnalogInput getAnalogInput() {
    return m_analog;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Gyro");
    builder.addDoubleProperty("Value", this::getAngle, null);
  }
}
