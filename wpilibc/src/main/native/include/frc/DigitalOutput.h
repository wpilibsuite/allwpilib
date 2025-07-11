// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/DIO.h>
#include <hal/Types.h>
#include <units/time.h>
#include <wpi/telemetry/TelemetryLoggable.h>

namespace frc {

/**
 * Class to write to digital outputs.
 *
 * Write values to the digital output channels. Other devices implemented
 * elsewhere will allocate channels automatically so for those devices it
 * shouldn't be done here.
 */
class DigitalOutput : public wpi::TelemetryLoggable {
 public:
  /**
   * Create an instance of a digital output.
   *
   * Create a digital output given a channel.
   *
   * @param channel The digital channel 0-9 are on-board, 10-25 are on the MXP
   *                port
   */
  explicit DigitalOutput(int channel);

  DigitalOutput(DigitalOutput&&) = default;
  DigitalOutput& operator=(DigitalOutput&&) = default;

  ~DigitalOutput() override;

  /**
   * Set the value of a digital output.
   *
   * Set the value of a digital output to either one (true) or zero (false).
   *
   * @param value 1 (true) for high, 0 (false) for disabled
   */
  void Set(bool value);

  /**
   * Gets the value being output from the Digital Output.
   *
   * @return the state of the digital output.
   */
  bool Get() const;

  /**
   * @return The GPIO channel number that this object represents.
   */
  int GetChannel() const;

  /**
   * Output a single pulse on the digital output line.
   *
   * Send a single pulse on the digital output line where the pulse duration is
   * specified in seconds. Maximum of 65535 microseconds.
   *
   * @param pulseLength The pulse length in seconds
   */
  void Pulse(units::second_t pulseLength);

  /**
   * Determine if the pulse is still going.
   *
   * Determine if a previously started pulse is still going.
   */
  bool IsPulsing() const;

  /**
   * Change the PWM frequency of the PWM output on a Digital Output line.
   *
   * The valid range is from 0.6 Hz to 19 kHz.  The frequency resolution is
   * logarithmic.
   *
   * There is only one PWM frequency for all digital channels.
   *
   * @param rate The frequency to output all digital output PWM signals.
   */
  void SetPWMRate(double rate);

  /**
   * Enable a PWM PPS (Pulse Per Second) Output on this line.
   *
   * Allocate one of the 6 DO PWM generator resources from this module.
   *
   * Supply the duty-cycle to output.
   *
   * The resolution of the duty cycle is 8-bit.
   *
   * @param dutyCycle The duty-cycle to start generating. [0..1]
   */
  void EnablePPS(double dutyCycle);

  /**
   * Enable a PWM Output on this line.
   *
   * Allocate one of the 6 DO PWM generator resources from this module.
   *
   * Supply the initial duty-cycle to output so as to avoid a glitch when first
   * starting.
   *
   * The resolution of the duty cycle is 8-bit for low frequencies (1kHz or
   * less) but is reduced the higher the frequency of the PWM signal is.
   *
   * @param initialDutyCycle The duty-cycle to start generating. [0..1]
   */
  void EnablePWM(double initialDutyCycle);

  /**
   * Change this line from a PWM output back to a static Digital Output line.
   *
   * Free up one of the 6 DO PWM generator resources that were in use.
   */
  void DisablePWM();

  /**
   * Change the duty-cycle that is being generated on the line.
   *
   * The resolution of the duty cycle is 8-bit for low frequencies (1kHz or
   * less) but is reduced the higher the frequency of the PWM signal is.
   *
   * @param dutyCycle The duty-cycle to change to. [0..1]
   */
  void UpdateDutyCycle(double dutyCycle);

  /**
   * Indicates this output is used by a simulated device.
   *
   * @param device simulated device handle
   */
  void SetSimDevice(HAL_SimDeviceHandle device);

  void UpdateTelemetry(wpi::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

 private:
  int m_channel;
  hal::Handle<HAL_DigitalHandle, HAL_FreeDIOPort> m_handle;
  hal::Handle<HAL_DigitalPWMHandle> m_pwmGenerator;
};

}  // namespace frc
