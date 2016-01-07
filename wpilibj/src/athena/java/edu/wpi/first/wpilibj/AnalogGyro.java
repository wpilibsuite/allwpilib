/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.interfaces.Gyro;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Use a rate gyro to return the robots heading relative to a starting position.
 * The Gyro class tracks the robots heading based on the starting position. As
 * the robot rotates the new heading is computed by integrating the rate of
 * rotation returned by the sensor. When the class is instantiated, it does a
 * short calibration routine where it samples the gyro while at rest to
 * determine the default offset. This is subtracted from each sample to
 * determine the heading.
 *
 * This class is for gyro sensors that connect to an analog input.
 */
public class AnalogGyro extends GyroBase implements Gyro, PIDSource, LiveWindowSendable {

  static final int kOversampleBits = 10;
  static final int kAverageBits = 0;
  static final double kSamplesPerSecond = 50.0;
  static final double kCalibrationSampleTime = 5.0;
  static final double kDefaultVoltsPerDegreePerSecond = 0.007;
  protected AnalogInput m_analog;
  double m_voltsPerDegreePerSecond;
  double m_offset;
  int m_center;
  boolean m_channelAllocated = false;
  AccumulatorResult result;
  private PIDSourceType m_pidSource;

  /**
   * Initialize the gyro. Calibration is handled by calibrate().
   */
  public void initGyro() {
    result = new AccumulatorResult();

    m_voltsPerDegreePerSecond = kDefaultVoltsPerDegreePerSecond;
    m_analog.setAverageBits(kAverageBits);
    m_analog.setOversampleBits(kOversampleBits);
    double sampleRate = kSamplesPerSecond * (1 << (kAverageBits + kOversampleBits));
    AnalogInput.setGlobalSampleRate(sampleRate);
    Timer.delay(0.1);

    setDeadband(0.0);

    setPIDSourceType(PIDSourceType.kDisplacement);

    UsageReporting.report(tResourceType.kResourceType_Gyro, m_analog.getChannel());
    LiveWindow.addSensor("AnalogGyro", m_analog.getChannel(), this);
  }

  /**
   * {@inheritDoc}
   */
  public void calibrate() {
    m_analog.initAccumulator();
    m_analog.resetAccumulator();

    Timer.delay(kCalibrationSampleTime);

    m_analog.getAccumulatorOutput(result);

    m_center = (int) ((double) result.value / (double) result.count + .5);

    m_offset = ((double) result.value / (double) result.count) - m_center;

    m_analog.setAccumulatorCenter(m_center);
    m_analog.resetAccumulator();
  }

  /**
   * Gyro constructor using the channel number
   *
   * @param channel The analog channel the gyro is connected to. Gyros can only
   *        be used on on-board channels 0-1.
   */
  public AnalogGyro(int channel) {
    this(new AnalogInput(channel));
    m_channelAllocated = true;
  }

  /**
   * Gyro constructor with a precreated analog channel object. Use this
   * constructor when the analog channel needs to be shared.
   *
   * @param channel The AnalogInput object that the gyro is connected to. Gyros
   *        can only be used on on-board channels 0-1.
   */
  public AnalogGyro(AnalogInput channel) {
    m_analog = channel;
    if (m_analog == null) {
      throw new NullPointerException("AnalogInput supplied to Gyro constructor is null");
    }
    initGyro();
    calibrate();
  }

  /**
   * Gyro constructor using the channel number along with parameters for
   * presetting the center and offset values. Bypasses calibration.
   * @param channel The analog channel the gyro is connected to. Gyros can only
   *        be used on on-board channels 0-1.
   * @param center Preset uncalibrated value to use as the accumulator center value.
   * @param offset Preset uncalibrated value to use as the gyro offset.
   */
  public AnalogGyro(int channel, int center, double offset) {
    this(new AnalogInput(channel), center, offset);
    m_channelAllocated = true;
  }

  /**
   * Gyro constructor with a precreated analog channel object along with
   * parameters for presetting the center and offset values. Bypasses
   * calibration.
   * @param channel The analog channel the gyro is connected to. Gyros can only
   *        be used on on-board channels 0-1.
   * @param center Preset uncalibrated value to use as the accumulator center value.
   * @param offset Preset uncalibrated value to use as the gyro offset.
   */
  public AnalogGyro(AnalogInput channel, int center, double offset) {
    m_analog = channel;
    if (m_analog == null) {
      throw new NullPointerException("AnalogInput supplied to Gyro constructor is null");
    }
    initGyro();
    m_offset = offset;
    m_center = center;

    m_analog.setAccumulatorCenter(m_center);
    m_analog.resetAccumulator();
  }

  /**
   * {@inheritDoc}
   */
  public void reset() {
    if (m_analog != null) {
      m_analog.resetAccumulator();
    }
  }

  /**
   * Delete (free) the accumulator and the analog components used for the gyro.
   */
  @Override
  public void free() {
    if (m_analog != null && m_channelAllocated) {
      m_analog.free();
    }
    m_analog = null;
  }

  /**
   * {@inheritDoc}
   */
  public double getAngle() {
    if (m_analog == null) {
      return 0.0;
    } else {
      m_analog.getAccumulatorOutput(result);

      long value = result.value - (long) (result.count * m_offset);

      double scaledValue =
          value * 1e-9 * m_analog.getLSBWeight() * (1 << m_analog.getAverageBits())
              / (AnalogInput.getGlobalSampleRate() * m_voltsPerDegreePerSecond);

      return scaledValue;
    }
  }

  /**
   * {@inheritDoc}
   */
  public double getRate() {
    if (m_analog == null) {
      return 0.0;
    } else {
      return (m_analog.getAverageValue() - (m_center + m_offset)) * 1e-9 * m_analog.getLSBWeight()
          / ((1 << m_analog.getOversampleBits()) * m_voltsPerDegreePerSecond);
    }
  }

  /**
   * Return the gyro offset value set during calibration to
   * use as a future preset
   *
   * @return the current offset value
   */
  public double getOffset() {
    return m_offset;
  }

  /**
   * Return the gyro center value set during calibration to
   * use as a future preset
   *
   * @return the current center value
   */
  public int getCenter() {
    return m_center;
  }

  /**
   * Set the gyro sensitivity. This takes the number of volts/degree/second
   * sensitivity of the gyro and uses it in subsequent calculations to allow the
   * code to work with multiple gyros. This value is typically found in the gyro
   * datasheet.
   *
   * @param voltsPerDegreePerSecond The sensitivity in Volts/degree/second.
   */
  public void setSensitivity(double voltsPerDegreePerSecond) {
    m_voltsPerDegreePerSecond = voltsPerDegreePerSecond;
  }

  /**
   * Set the size of the neutral zone. Any voltage from the gyro less than this
   * amount from the center is considered stationary. Setting a deadband will
   * decrease the amount of drift when the gyro isn't rotating, but will make it
   * less accurate.
   *
   * @param volts The size of the deadband in volts
   */
  void setDeadband(double volts) {
    int deadband =
        (int) (volts * 1e9 / m_analog.getLSBWeight() * (1 << m_analog.getOversampleBits()));
    m_analog.setAccumulatorDeadband(deadband);
  }
}
