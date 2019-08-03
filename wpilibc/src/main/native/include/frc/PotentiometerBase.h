/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/interfaces/Potentiometer.h"
#include "frc/smartdashboard/SendableBase.h"

namespace frc {

/**
 * Interface for potentiometers that can be displayed on a dashboard.
 */
class PotentiometerBase : public Potentiometer, public SendableBase {
 public:
  PotentiometerBase() = default;
  virtual ~PotentiometerBase() = default;

  PotentiometerBase(PotentiometerBase&&) = default;
  PotentiometerBase& operator=(PotentiometerBase&&) = default;
};

}  // namespace frc
