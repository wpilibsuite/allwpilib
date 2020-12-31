// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/DigitalSource.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class DigitalGlitchFilter;
class SendableBuilder;

/**
 * Class to read a digital input.
 *
 * This class will read digital inputs and return the current value on the
 * channel. Other devices such as encoders, gear tooth sensors, etc. that are
 * implemented elsewhere will automatically allocate digital inputs and outputs
 * as required. This class is only for devices like switches etc. that aren't
 * implemented anywhere else.
 */
class DigitalInput : public DigitalSource,
                     public Sendable,
                     public SendableHelper<DigitalInput> {
 public:
  /**
   * Create an instance of a Digital Input class.
   *
   * Creates a digital input given a channel.
   *
   * @param channel The DIO channel 0-9 are on-board, 10-25 are on the MXP port
   */
  explicit DigitalInput(int channel);

  ~DigitalInput() override;

  DigitalInput(DigitalInput&&) = default;
  DigitalInput& operator=(DigitalInput&&) = default;

  /**
   * Get the value from a digital input channel.
   *
   * Retrieve the value of a single digital input channel from the FPGA.
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
   * Indicates this input is used by a simulated device.
   *
   * @param device simulated device handle
   */
  void SetSimDevice(HAL_SimDeviceHandle device);

  void InitSendable(SendableBuilder& builder) override;

 private:
  int m_channel;
  hal::Handle<HAL_DigitalHandle> m_handle;

  friend class DigitalGlitchFilter;
};

}  // namespace frc
