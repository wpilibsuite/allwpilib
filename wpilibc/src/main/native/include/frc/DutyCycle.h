// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/DutyCycle.h>
#include <hal/Types.h>
#include <units/time.h>
#include <wpi/telemetry/TelemetryLoggable.h>

namespace frc {
/**
 * Class to read a duty cycle PWM input.
 *
 * <p>PWM input signals are specified with a frequency and a ratio of high to
 * low in that frequency. There are 8 of these in the roboRIO, and they can be
 * attached to any DigitalSource.
 *
 * <p>These can be combined as the input of an AnalogTrigger to a Counter in
 * order to implement rollover checking.
 *
 */
class DutyCycle : public wpi::TelemetryLoggable {
 public:
  /**
   * Constructs a DutyCycle input from a smartio channel.
   *
   * @param source The channel to use.
   */
  explicit DutyCycle(int source);

  DutyCycle(DutyCycle&&) = default;
  DutyCycle& operator=(DutyCycle&&) = default;

  /**
   * Close the DutyCycle and free all resources.
   */
  ~DutyCycle() override = default;

  /**
   * Get the frequency of the duty cycle signal.
   *
   * @return frequency in Hertz
   */
  int GetFrequency() const;

  /**
   * Get the output ratio of the duty cycle signal.
   *
   * <p> 0 means always low, 1 means always high.
   *
   * @return output ratio between 0 and 1
   */
  double GetOutput() const;

  /**
   * Get the raw high time of the duty cycle signal.
   *
   * @return high time of last pulse
   */
  units::second_t GetHighTime() const;

  /**
   * Get the scale factor of the output.
   *
   * <p> An output equal to this value is always high, and then linearly scales
   * down to 0. Divide a raw result by this in order to get the
   * percentage between 0 and 1. Used by DMA.
   *
   * @return the output scale factor
   */
  unsigned int GetOutputScaleFactor() const;

  /**
   * Get the FPGA index for the DutyCycle.
   *
   * @return the FPGA index
   */
  int GetFPGAIndex() const;

  /**
   * Get the channel of the source.
   *
   * @return the source channel
   */
  int GetSourceChannel() const;

  void UpdateTelemetry(wpi::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

 private:
  void InitDutyCycle();
  int m_channel;
  hal::Handle<HAL_DutyCycleHandle, HAL_FreeDutyCycle> m_handle;
};
}  // namespace frc
