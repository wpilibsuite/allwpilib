/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "lowfisim/SimulatorComponentBase.h"

namespace frc {
namespace sim {
namespace lowfi {

class MotorSim : public SimulatorComponentBase {
 public:
  virtual double GetPosition() const = 0;
  virtual double GetVelocity() const = 0;
  virtual double GetAcceleration() const = 0;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
