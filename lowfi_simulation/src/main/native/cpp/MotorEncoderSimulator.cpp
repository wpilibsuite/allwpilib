/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MotorEncoderSimulator.h"

namespace frc {
namespace sim {
namespace lowfi {

MotorEncoderSimulator::MotorEncoderSimulator(
    const std::shared_ptr<MotorSimulator>& motorController,
    const std::shared_ptr<EncoderSimulator>& encoder)
    : motorSimulator(motorController), encoderSimulator(encoder) {}

MotorEncoderSimulator::~MotorEncoderSimulator() {}

void MotorEncoderSimulator::Update() {
  encoderSimulator->SetPosition(motorSimulator->GetPosition());
}

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
