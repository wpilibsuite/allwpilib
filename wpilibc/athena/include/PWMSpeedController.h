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
  void Set(float value) override;
  float Get() const override;
  void Disable() override;
  void StopMotor() override;

  void PIDWrite(float output) override;

  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;

 protected:
  explicit PWMSpeedController(uint32_t channel);

 private:
  bool m_isInverted = false;
};
