/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/DigitalSource.h"

namespace frc {

class DigitalGlitchFilter;

/**
 * Class to read a digital input.
 *
 * This class will read digital inputs and return the current value on the
 * channel. Other devices such as encoders, gear tooth sensors, etc. that are
 * implemented elsewhere will automatically allocate digital inputs and outputs
 * as required. This class is only for devices like switches etc. that aren't
 * implemented anywhere else.
 */
class DigitalInput : public DigitalSource {
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

  DigitalInput(DigitalInput&& rhs);
  DigitalInput& operator=(DigitalInput&& rhs);

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

  void InitSendable(SendableBuilder& builder) override;

 private:
  int m_channel;
  HAL_DigitalHandle m_handle = HAL_kInvalidHandle;

  friend class DigitalGlitchFilter;
};

}  // namespace frc
