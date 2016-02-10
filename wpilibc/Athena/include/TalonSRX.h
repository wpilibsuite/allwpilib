/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "SafePWM.h"
#include "SpeedController.h"
#include "PIDOutput.h"

/**
 * Cross the Road Electronics (CTRE) Talon SRX Speed Controller with PWM control
 * @see CANTalon for CAN control
 */
class TalonSRX : public SafePWM, public SpeedController {
 public:
  explicit TalonSRX(uint32_t channel);
  virtual ~TalonSRX() = default;
  virtual void Set(float value, uint8_t syncGroup = 0) override;
  virtual float Get() const override;
  virtual void Disable() override;
  virtual void StopMotor() override;

  virtual void PIDWrite(float output) override;
  virtual void SetInverted(bool isInverted) override;
  virtual bool GetInverted() const override;

 private:
  bool m_isInverted = false;
};
