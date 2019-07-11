/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <hal/Types.h>

#include "frc/ErrorBase.h"
#include "frc/PIDSource.h"
#include "frc/smartdashboard/SendableBase.h"

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
class AnalogInput : public ErrorBase, public SendableBase, public PIDSource {
  friend class AnalogTrigger;
  friend class AnalogGyro;

 public:
  static constexpr int kAccumulatorModuleNumber = 1;
  static constexpr int kAccumulatorNumChannels = 2;
  static constexpr int kAccumulatorChannels[kAccumulatorNumChannels] = {0, 1};

  /**
   * Construct an analog input.
   *
   * @param channel The channel number on the roboRIO to represent. 0-3 are
   *                on-board 4-7 are on the MXP port.
   */
  explicit AnalogInput(int channel);

  ~AnalogInput() override;

  AnalogInput(AnalogInput&& rhs);
  AnalogInput& operator=(AnalogInput&& rhs);

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
   * Is the channel attached to an accumulator.
   *
   * @return The analog input is attached to an accumulator.
   */
  bool IsAccumulatorChannel() const;

  /**
   * Initialize the accumulator.
   */
  void InitAccumulator();

  /**
   * Set an initial value for the accumulator.
   *
   * This will be added to all values returned to the user.
   *
   * @param initialValue The value that the accumulator should start from when
   *                     reset.
   */
  void SetAccumulatorInitialValue(int64_t value);

  /**
   * Resets the accumulator to the initial value.
   */
  void ResetAccumulator();

  /**
   * Set the center value of the accumulator.
   *
   * The center value is subtracted from each A/D value before it is added to
   * the accumulator. This is used for the center value of devices like gyros
   * and accelerometers to take the device offset into account when integrating.
   *
   * This center value is based on the output of the oversampled and averaged
   * source from the accumulator channel. Because of this, any non-zero
   * oversample bits will affect the size of the value for this field.
   */
  void SetAccumulatorCenter(int center);

  /**
   * Set the accumulator's deadband.
   */
  void SetAccumulatorDeadband(int deadband);

  /**
   * Read the accumulated value.
   *
   * Read the value that has been accumulating.
   * The accumulator is attached after the oversample and average engine.
   *
   * @return The 64-bit value accumulated since the last Reset().
   */
  int64_t GetAccumulatorValue() const;

  /**
   * Read the number of accumulated values.
   *
   * Read the count of the accumulated values since the accumulator was last
   * Reset().
   *
   * @return The number of times samples from the channel were accumulated.
   */
  int64_t GetAccumulatorCount() const;

  /**
   * Read the accumulated value and the number of accumulated values atomically.
   *
   * This function reads the value and count from the FPGA atomically.
   * This can be used for averaging.
   *
   * @param value Reference to the 64-bit accumulated output.
   * @param count Reference to the number of accumulation cycles.
   */
  void GetAccumulatorOutput(int64_t& value, int64_t& count) const;

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
   * Get the Average value for the PID Source base object.
   *
   * @return The average voltage.
   */
  double PIDGet() override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  int m_channel;
  HAL_AnalogInputHandle m_port = HAL_kInvalidHandle;
  int64_t m_accumulatorOffset;
};

}  // namespace frc
