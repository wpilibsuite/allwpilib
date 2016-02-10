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
 * Mindsensors SD540 Speed Controller
 */
class SD540 : public SafePWM, public SpeedController {
 public:
  explicit SD540(uint32_t channel);
  virtual ~SD540() = default;
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
