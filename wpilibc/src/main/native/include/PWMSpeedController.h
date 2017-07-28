/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "SafePWM.h"
#include "SpeedController.h"

namespace frc {

/**
 * Common base class for all PWM Speed Controllers
 */
class PWMSpeedController : public SafePWM, public SpeedController {
 public:
  virtual ~PWMSpeedController() = default;
  void Set(double value) override;
  double Get() const override;
  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;
  void StopMotor() override;

  void PIDWrite(double output) override;

 protected:
  explicit PWMSpeedController(int channel);

 private:
  bool m_isInverted = false;
};

}  // namespace frc
