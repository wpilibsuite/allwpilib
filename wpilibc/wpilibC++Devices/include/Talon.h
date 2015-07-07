/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SafePWM.h"
#include "SpeedController.h"
#include "PIDOutput.h"

/**
 * Cross the Road Electronics (CTRE) Talon and Talon SR Speed Controller
 */
class Talon : public SafePWM, public SpeedController {
 public:
  explicit Talon(uint32_t channel);
  virtual ~Talon() = default;
  virtual void Set(float value, uint8_t syncGroup = 0) override;
  virtual float Get() const override;
  virtual void Disable() override;

  virtual void PIDWrite(float output) override;
  virtual void SetInverted(bool isInverted) override;
  virtual bool GetInverted() const override;

 private:
  bool m_isInverted = false;
};
