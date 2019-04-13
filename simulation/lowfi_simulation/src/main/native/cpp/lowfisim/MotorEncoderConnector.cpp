/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "lowfisim/MotorEncoderConnector.h"

namespace frc {
namespace sim {
namespace lowfi {

MotorEncoderConnector::MotorEncoderConnector(MotorSim& motorController,
                                             EncoderSim& encoder)
    : motorSimulator(motorController), encoderSimulator(encoder) {}

void MotorEncoderConnector::Update() {
  encoderSimulator.SetPosition(motorSimulator.GetPosition());
  encoderSimulator.SetVelocity(motorSimulator.GetVelocity());
}

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
