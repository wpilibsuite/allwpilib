// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/DutyCycle.h>
#include <hal/Types.h>
#include <units/time.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {
class DigitalSource;
class AnalogTrigger;
class DMA;
class DMASample;

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
class DutyCycle : public wpi::Sendable, public wpi::SendableHelper<DutyCycle> {
  friend class AnalogTrigger;
  friend class DMA;
  friend class DMASample;

 public:
  /**
   * Constructs a DutyCycle input from a DigitalSource input.
   *
   * <p> This class does not own the inputted source.
   *
   * @param source The DigitalSource to use.
   */
  explicit DutyCycle(DigitalSource& source);
  /**
   * Constructs a DutyCycle input from a DigitalSource input.
   *
   * <p> This class does not own the inputted source.
   *
   * @param source The DigitalSource to use.
   */
  explicit DutyCycle(DigitalSource* source);
  /**
   * Constructs a DutyCycle input from a DigitalSource input.
   *
   * <p> This class does not own the inputted source.
   *
   * @param source The DigitalSource to use.
   */
  explicit DutyCycle(std::shared_ptr<DigitalSource> source);

  DutyCycle(DutyCycle&&) = default;
  DutyCycle& operator=(DutyCycle&&) = default;

  /**
   * Close the DutyCycle and free all resources.
   */
  ~DutyCycle() override = default;

  /**
   * Get the frequency of the duty cycle signal.
   *
   * @warning This will return inaccurate values for up to 2 seconds after this
   * duty cycle input is initialized.
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

 protected:
  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  void InitDutyCycle();
  std::shared_ptr<DigitalSource> m_source;
  hal::Handle<HAL_DutyCycleHandle, HAL_FreeDutyCycle> m_handle;
};
}  // namespace frc
