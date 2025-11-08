// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/hal/DutyCycle.h"
#include "wpi/hal/Types.h"
#include "wpi/units/frequency.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/sendable/Sendable.hpp"
#include "wpi/util/sendable/SendableHelper.hpp"

namespace wpi {
/**
 * Class to read a duty cycle PWM input.
 *
 * <p>PWM input signals are specified with a frequency and a ratio of high to
 * low in that frequency. These can be attached to any SmartIO.
 *
 */
class DutyCycle : public wpi::util::Sendable,
                  public wpi::util::SendableHelper<DutyCycle> {
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
   * @return frequency
   */
  wpi::units::hertz_t GetFrequency() const;

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
  wpi::units::second_t GetHighTime() const;

  /**
   * Get the channel of the source.
   *
   * @return the source channel
   */
  int GetSourceChannel() const;

 protected:
  void InitSendable(wpi::util::SendableBuilder& builder) override;

 private:
  void InitDutyCycle();
  int m_channel;
  wpi::hal::Handle<HAL_DutyCycleHandle, HAL_FreeDutyCycle> m_handle;
};
}  // namespace wpi
