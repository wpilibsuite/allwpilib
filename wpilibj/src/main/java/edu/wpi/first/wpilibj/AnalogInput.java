// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AccumulatorResult;
import edu.wpi.first.hal.AnalogJNI;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.util.AllocationException;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Analog channel class.
 *
 * <p>Each analog channel is read from hardware as a 12-bit number representing 0V to 5V.
 *
 * <p>Connected to each analog channel is an averaging and oversampling engine. This engine
 * accumulates the specified ( by setAverageBits() and setOversampleBits() ) number of samples
 * before returning a new value. This is not a sliding window average. The only difference between
 * the oversampled samples and the averaged samples is that the oversampled samples are simply
 * accumulated effectively increasing the resolution, while the averaged samples are divided by the
 * number of samples to retain the resolution, but get more stable values.
 */
public class AnalogInput implements Sendable, AutoCloseable {
  private static final int kAccumulatorSlot = 1;
  int m_port; // explicit no modifier, private and package accessible.
  private int m_channel;
  private static final int[] kAccumulatorChannels = {0, 1};
  private long m_accumulatorOffset;

  /**
   * Construct an analog channel.
   *
   * @param channel The channel number to represent. 0-3 are on-board 4-7 are on the MXP port.
   */
  @SuppressWarnings("this-escape")
  public AnalogInput(final int channel) {
    AnalogJNI.checkAnalogInputChannel(channel);
    m_channel = channel;

    final int portHandle = HAL.getPort((byte) channel);
    m_port = AnalogJNI.initializeAnalogInputPort(portHandle);

    HAL.report(tResourceType.kResourceType_AnalogChannel, channel + 1);
    SendableRegistry.addLW(this, "AnalogInput", channel);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    AnalogJNI.freeAnalogInputPort(m_port);
    m_port = 0;
    m_channel = 0;
    m_accumulatorOffset = 0;
  }

  /**
   * Get a sample straight from this channel. The sample is a 12-bit value representing the 0V to 5V
   * range of the A/D converter. The units are in A/D converter codes. Use GetVoltage() to get the
   * analog value in calibrated units.
   *
   * @return A sample straight from this channel.
   */
  public int getValue() {
    return AnalogJNI.getAnalogValue(m_port);
  }

  /**
   * Get a sample from the output of the oversample and average engine for this channel. The sample
   * is 12-bit + the bits configured in SetOversampleBits(). The value configured in
   * setAverageBits() will cause this value to be averaged 2^bits number of samples. This is not a
   * sliding window. The sample will not change until 2^(OversampleBits + AverageBits) samples have
   * been acquired from this channel. Use getAverageVoltage() to get the analog value in calibrated
   * units.
   *
   * @return A sample from the oversample and average engine for this channel.
   */
  public int getAverageValue() {
    return AnalogJNI.getAnalogAverageValue(m_port);
  }

  /**
   * Get a scaled sample straight from this channel. The value is scaled to units of Volts using the
   * calibrated scaling data from getLSBWeight() and getOffset().
   *
   * @return A scaled sample straight from this channel.
   */
  public double getVoltage() {
    return AnalogJNI.getAnalogVoltage(m_port);
  }

  /**
   * Get a scaled sample from the output of the oversample and average engine for this channel. The
   * value is scaled to units of Volts using the calibrated scaling data from getLSBWeight() and
   * getOffset(). Using oversampling will cause this value to be higher resolution, but it will
   * update more slowly. Using averaging will cause this value to be more stable, but it will update
   * more slowly.
   *
   * @return A scaled sample from the output of the oversample and average engine for this channel.
   */
  public double getAverageVoltage() {
    return AnalogJNI.getAnalogAverageVoltage(m_port);
  }

  /**
   * Get the factory scaling the least significant bit weight constant. The least significant bit
   * weight constant for the channel that was calibrated in manufacturing and stored in an eeprom.
   *
   * <p>Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
   *
   * @return Least significant bit weight.
   */
  public long getLSBWeight() {
    return AnalogJNI.getAnalogLSBWeight(m_port);
  }

  /**
   * Get the factory scaling offset constant. The offset constant for the channel that was
   * calibrated in manufacturing and stored in an eeprom.
   *
   * <p>Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
   *
   * @return Offset constant.
   */
  public int getOffset() {
    return AnalogJNI.getAnalogOffset(m_port);
  }

  /**
   * Get the channel number.
   *
   * @return The channel number.
   */
  public int getChannel() {
    return m_channel;
  }

  /**
   * Set the number of averaging bits. This sets the number of averaging bits. The actual number of
   * averaged samples is 2^bits. The averaging is done automatically in the FPGA.
   *
   * @param bits The number of averaging bits.
   */
  public void setAverageBits(final int bits) {
    AnalogJNI.setAnalogAverageBits(m_port, bits);
  }

  /**
   * Get the number of averaging bits. This gets the number of averaging bits from the FPGA. The
   * actual number of averaged samples is 2^bits. The averaging is done automatically in the FPGA.
   *
   * @return The number of averaging bits.
   */
  public int getAverageBits() {
    return AnalogJNI.getAnalogAverageBits(m_port);
  }

  /**
   * Set the number of oversample bits. This sets the number of oversample bits. The actual number
   * of oversampled values is 2^bits. The oversampling is done automatically in the FPGA.
   *
   * @param bits The number of oversample bits.
   */
  public void setOversampleBits(final int bits) {
    AnalogJNI.setAnalogOversampleBits(m_port, bits);
  }

  /**
   * Get the number of oversample bits. This gets the number of oversample bits from the FPGA. The
   * actual number of oversampled values is 2^bits. The oversampling is done automatically in the
   * FPGA.
   *
   * @return The number of oversample bits.
   */
  public int getOversampleBits() {
    return AnalogJNI.getAnalogOversampleBits(m_port);
  }

  /** Initialize the accumulator. */
  public void initAccumulator() {
    if (!isAccumulatorChannel()) {
      throw new AllocationException("Accumulators are only available on slot "
          + kAccumulatorSlot
          + " on channels "
          + kAccumulatorChannels[0]
          + ", "
          + kAccumulatorChannels[1]);
    }
    m_accumulatorOffset = 0;
    AnalogJNI.initAccumulator(m_port);
  }

  /**
   * Set an initial value for the accumulator.
   *
   * <p>This will be added to all values returned to the user.
   *
   * @param initialValue The value that the accumulator should start from when reset.
   */
  public void setAccumulatorInitialValue(long initialValue) {
    m_accumulatorOffset = initialValue;
  }

  /** Resets the accumulator to the initial value. */
  public void resetAccumulator() {
    AnalogJNI.resetAccumulator(m_port);

    // Wait until the next sample, so the next call to getAccumulator*()
    // won't have old values.
    final double sampleTime = 1.0 / getGlobalSampleRate();
    final double overSamples = 1 << getOversampleBits();
    final double averageSamples = 1 << getAverageBits();
    Timer.delay(sampleTime * overSamples * averageSamples);
  }

  /**
   * Set the center value of the accumulator.
   *
   * <p>The center value is subtracted from each A/D value before it is added to the accumulator.
   * This is used for the center value of devices like gyros and accelerometers to take the device
   * offset into account when integrating.
   *
   * <p>This center value is based on the output of the oversampled and averaged source the
   * accumulator channel. Because of this, any non-zero oversample bits will affect the size of the
   * value for this field.
   *
   * @param center The accumulator's center value.
   */
  public void setAccumulatorCenter(int center) {
    AnalogJNI.setAccumulatorCenter(m_port, center);
  }

  /**
   * Set the accumulator's deadband.
   *
   * @param deadband The deadband size in ADC codes (12-bit value)
   */
  public void setAccumulatorDeadband(int deadband) {
    AnalogJNI.setAccumulatorDeadband(m_port, deadband);
  }

  /**
   * Read the accumulated value.
   *
   * <p>Read the value that has been accumulating. The accumulator is attached after the oversample
   * and average engine.
   *
   * @return The 64-bit value accumulated since the last Reset().
   */
  public long getAccumulatorValue() {
    return AnalogJNI.getAccumulatorValue(m_port) + m_accumulatorOffset;
  }

  /**
   * Read the number of accumulated values.
   *
   * <p>Read the count of the accumulated values since the accumulator was last Reset().
   *
   * @return The number of times samples from the channel were accumulated.
   */
  public long getAccumulatorCount() {
    return AnalogJNI.getAccumulatorCount(m_port);
  }

  /**
   * Read the accumulated value and the number of accumulated values atomically.
   *
   * <p>This function reads the value and count from the FPGA atomically. This can be used for
   * averaging.
   *
   * @param result AccumulatorResult object to store the results in.
   */
  public void getAccumulatorOutput(AccumulatorResult result) {
    if (result == null) {
      throw new IllegalArgumentException("Null parameter `result'");
    }
    if (!isAccumulatorChannel()) {
      throw new IllegalArgumentException(
          "Channel " + m_channel + " is not an accumulator channel.");
    }
    AnalogJNI.getAccumulatorOutput(m_port, result);
    result.value += m_accumulatorOffset;
  }

  /**
   * Is the channel attached to an accumulator.
   *
   * @return The analog channel is attached to an accumulator.
   */
  public boolean isAccumulatorChannel() {
    for (int channel : kAccumulatorChannels) {
      if (m_channel == channel) {
        return true;
      }
    }
    return false;
  }

  /**
   * Set the sample rate per channel.
   *
   * <p>This is a global setting for all channels. The maximum rate is 500kS/s divided by the number
   * of channels in use. This is 62500 samples/s per channel if all 8 channels are used.
   *
   * @param samplesPerSecond The number of samples per second.
   */
  public static void setGlobalSampleRate(final double samplesPerSecond) {
    AnalogJNI.setAnalogSampleRate(samplesPerSecond);
  }

  /**
   * Get the current sample rate.
   *
   * <p>This assumes one entry in the scan list. This is a global setting for all channels.
   *
   * @return Sample rate.
   */
  public static double getGlobalSampleRate() {
    return AnalogJNI.getAnalogSampleRate();
  }

  /**
   * Indicates this input is used by a simulated device.
   *
   * @param device simulated device handle
   */
  public void setSimDevice(SimDevice device) {
    AnalogJNI.setAnalogInputSimDevice(m_port, device.getNativeHandle());
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Analog Input");
    builder.addDoubleProperty("Value", this::getAverageVoltage, null);
  }
}
