/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <HAL/Types.h>

#include "ErrorBase.h"
#include "SmartDashboard/SendableBase.h"

namespace frc {

/**
 * Class to write to digital outputs.
 *
 * Write values to the digital output channels. Other devices implemented
 * elsewhere will allocate channels automatically so for those devices it
 * shouldn't be done here.
 */
class DigitalOutput : public ErrorBase, public SendableBase {
 public:
  explicit DigitalOutput(int channel);
  ~DigitalOutput() override;
  void Set(bool value);
  bool Get() const;
  int GetChannel() const;
  void Pulse(double length);
  bool IsPulsing() const;
  void SetPWMRate(double rate);
  void EnablePWM(double initialDutyCycle);
  void DisablePWM();
  void UpdateDutyCycle(double dutyCycle);

  void InitSendable(SendableBuilder& builder) override;

 private:
  int m_channel;
  HAL_DigitalHandle m_handle;
  HAL_DigitalPWMHandle m_pwmGenerator;
};

}  // namespace frc
