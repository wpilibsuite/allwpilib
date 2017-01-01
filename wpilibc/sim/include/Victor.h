/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "PIDOutput.h"
#include "SafePWM.h"
#include "SpeedController.h"

namespace frc {

/**
 * IFI Victor Speed Controller.
 */
class Victor : public SafePWM, public SpeedController {
 public:
  explicit Victor(int channel);
  virtual ~Victor() = default;
  virtual void Set(double value);
  virtual double Get() const;
  virtual void Disable();

  void PIDWrite(double output) override;
};

}  // namespace frc
