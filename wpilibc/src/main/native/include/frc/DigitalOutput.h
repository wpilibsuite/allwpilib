/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/Types.h>

#include "frc/DigitalSource.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableBuilder;

/**
 * Class to write to digital outputs.
 *
 * Write values to the digital output channels. Other devices implemented
 * elsewhere will allocate channels automatically so for those devices it
 * shouldn't be done here.
 */
class DigitalOutput : public DigitalSource,
                      public Sendable,
                      public SendableHelper<DigitalOutput> {
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

  ~DigitalOutput() override;

  DigitalOutput(DigitalOutput&&) = default;
  DigitalOutput& operator=(DigitalOutput&&) = default;

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

  // Digital Source Interface
  /**
   * @return The HAL Handle to the specified source.
   */
  HAL_Handle GetPortHandleForRouting() const override;

  /**
   * @return The type of analog trigger output to be used. 0 for Digitals
   */
  AnalogTriggerType GetAnalogTriggerTypeForRouting() const override;

  /**
   * Is source an AnalogTrigger
   */
  bool IsAnalogTrigger() const override;

  /**
   * @return The GPIO channel number that this object represents.
   */
  int GetChannel() const override;

  /**
   * Output a single pulse on the digital output line.
   *
   * Send a single pulse on the digital output line where the pulse duration is
   * specified in seconds. Maximum pulse length is 0.0016 seconds.
   *
   * @param length The pulse length in seconds
   */
  void Pulse(double length);

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

  void InitSendable(SendableBuilder& builder) override;

 private:
  int m_channel;
  hal::Handle<HAL_DigitalHandle> m_handle;
  hal::Handle<HAL_DigitalPWMHandle> m_pwmGenerator;
};

}  // namespace frc
