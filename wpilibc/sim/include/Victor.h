/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "PIDOutput.h"
#include "SafePWM.h"
#include "SpeedController.h"

/**
 * IFI Victor Speed Controller.
 */
class Victor : public SafePWM, public SpeedController {
 public:
  explicit Victor(uint32_t channel);
  virtual ~Victor() = default;
  virtual void Set(float value);
  virtual float Get() const;
  virtual void Disable();

  virtual void PIDWrite(float output) override;
};
