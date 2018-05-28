/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "EncoderSimulator.h"
#include "MotorSimulator.h"

namespace frc {
namespace sim {
namespace lowfi {

class MotorEncoderSimulator {
 public:
  MotorEncoderSimulator(const std::shared_ptr<MotorSimulator>& motorController,
                        const std::shared_ptr<EncoderSimulator>& encoder);
  ~MotorEncoderSimulator();

  void Update();

 protected:
  std::shared_ptr<MotorSimulator> motorSimulator;
  std::shared_ptr<EncoderSimulator> encoderSimulator;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
