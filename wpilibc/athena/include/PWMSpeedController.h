/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "SafePWM.h"
#include "SpeedController.h"

/**
 * Common base class for all PWM Speed Controllers
 */
class PWMSpeedController : public SafePWM, public SpeedController {
 public:
  virtual ~PWMSpeedController() = default;
  virtual void Set(float value) override;
  virtual float Get() const override;
  virtual void Disable() override;
  virtual void StopMotor() override;

  virtual void PIDWrite(float output) override;

  virtual void SetInverted(bool isInverted) override;
  virtual bool GetInverted() const override;

 protected:
  explicit PWMSpeedController(uint32_t channel);

 private:
  bool m_isInverted = false;
};
