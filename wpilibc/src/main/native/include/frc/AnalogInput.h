// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <hal/AnalogInput.h>
#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {

/**
 * Analog input class.
 *
 * Connected to each analog channel is an averaging and oversampling engine.
 * This engine accumulates the specified ( by SetAverageBits() and
 * SetOversampleBits() ) number of samples before returning a new value. This is
 * not a sliding window average. The only difference between the oversampled
 * samples and the averaged samples is that the oversampled samples are simply
 * accumulated effectively increasing the resolution, while the averaged samples
 * are divided by the number of samples to retain the resolution, but get more
 * stable values.
 */
class AnalogInput : public wpi::Sendable,
                    public wpi::SendableHelper<AnalogInput> {
 public:
  /**
   * Construct an analog input.
   *
   * @param channel The channel number on the roboRIO to represent. 0-3 are
   *                on-board 4-7 are on the MXP port.
   */
  explicit AnalogInput(int channel);

  AnalogInput(AnalogInput&&) = default;
  AnalogInput& operator=(AnalogInput&&) = default;

  ~AnalogInput() override = default;

  /**
   * Get a sample straight from this channel.
   *
   * The sample is a 12-bit value representing the 0V to 5V range of the A/D
   * converter in the module.  The units are in A/D converter codes.  Use
   * GetVoltage() to get the analog value in calibrated units.
   *
   * @return A sample straight from this channel.
   */
  int GetValue() const;

  /**
   * Get a sample from the output of the oversample and average engine for this
   * channel.
   *
   * The sample is 12-bit + the bits configured in SetOversampleBits().
   * The value configured in SetAverageBits() will cause this value to be
   * averaged 2**bits number of samples.
   *
   * This is not a sliding window. The sample will not change until
   * 2**(OversampleBits + AverageBits) samples have been acquired from the
   * module on this channel.
   *
   * Use GetAverageVoltage() to get the analog value in calibrated units.
   *
   * @return A sample from the oversample and average engine for this channel.
   */
  int GetAverageValue() const;

  /**
   * Get a scaled sample straight from this channel.
   *
   * The value is scaled to units of Volts using the calibrated scaling data
   * from GetLSBWeight() and GetOffset().
   *
   * @return A scaled sample straight from this channel.
   */
  double GetVoltage() const;

  /**
   * Get a scaled sample from the output of the oversample and average engine
   * for this channel.
   *
   * The value is scaled to units of Volts using the calibrated scaling data
   * from GetLSBWeight() and GetOffset().
   *
   * Using oversampling will cause this value to be higher resolution, but it
   * will update more slowly.
   *
   * Using averaging will cause this value to be more stable, but it will update
   * more slowly.
   *
   * @return A scaled sample from the output of the oversample and average
   * engine for this channel.
   */
  double GetAverageVoltage() const;

  /**
   * Get the channel number.
   *
   * @return The channel number.
   */
  int GetChannel() const;

  /**
   * Set the number of averaging bits.
   *
   * This sets the number of averaging bits. The actual number of averaged
   * samples is 2^bits.
   *
   * Use averaging to improve the stability of your measurement at the expense
   * of sampling rate. The averaging is done automatically in the FPGA.
   *
   * @param bits Number of bits of averaging.
   */
  void SetAverageBits(int bits);

  /**
   * Get the number of averaging bits previously configured.
   *
   * This gets the number of averaging bits from the FPGA. The actual number of
   * averaged samples is 2^bits. The averaging is done automatically in the
   * FPGA.
   *
   * @return Number of bits of averaging previously configured.
   */
  int GetAverageBits() const;

  /**
   * Set the number of oversample bits.
   *
   * This sets the number of oversample bits. The actual number of oversampled
   * values is 2^bits. Use oversampling to improve the resolution of your
   * measurements at the expense of sampling rate. The oversampling is done
   * automatically in the FPGA.
   *
   * @param bits Number of bits of oversampling.
   */
  void SetOversampleBits(int bits);

  /**
   * Get the number of oversample bits previously configured.
   *
   * This gets the number of oversample bits from the FPGA. The actual number of
   * oversampled values is 2^bits. The oversampling is done automatically in the
   * FPGA.
   *
   * @return Number of bits of oversampling previously configured.
   */
  int GetOversampleBits() const;

  /**
   * Get the factory scaling least significant bit weight constant.
   *
   * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
   *
   * @return Least significant bit weight.
   */
  int GetLSBWeight() const;

  /**
   * Get the factory scaling offset constant.
   *
   * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
   *
   * @return Offset constant.
   */
  int GetOffset() const;

  /**
   * Set the sample rate per channel for all analog channels.
   *
   * The maximum rate is 500kS/s divided by the number of channels in use.
   * This is 62500 samples/s per channel.
   *
   * @param samplesPerSecond The number of samples per second.
   */
  static void SetSampleRate(double samplesPerSecond);

  /**
   * Get the current sample rate for all channels
   *
   * @return Sample rate.
   */
  static double GetSampleRate();

  /**
   * Indicates this input is used by a simulated device.
   *
   * @param device simulated device handle
   */
  void SetSimDevice(HAL_SimDeviceHandle device);

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  int m_channel;
  hal::Handle<HAL_AnalogInputHandle, HAL_FreeAnalogInputPort> m_port;
};

}  // namespace frc
