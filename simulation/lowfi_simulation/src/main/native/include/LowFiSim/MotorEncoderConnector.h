/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "EncoderSim.h"
#include "MotorSim.h"

namespace frc {
namespace sim {
namespace lowfi {

class MotorEncoderConnector {
 public:
  MotorEncoderConnector(MotorSim& motorController, EncoderSim& encoder);

  void Update();

 private:
  MotorSim& motorSimulator;
  EncoderSim& encoderSimulator;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
