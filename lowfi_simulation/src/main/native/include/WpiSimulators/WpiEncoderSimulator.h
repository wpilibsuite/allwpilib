/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "EncoderSimulator.h"
#include "Simulation/EncoderSim.h"

namespace frc {
namespace sim {
namespace lowfi {

class WpiEncoderSimulator : public EncoderSimulator {
 public:
  explicit WpiEncoderSimulator(int index);
  ~WpiEncoderSimulator();

  void SetPosition(double position) override;
  void SetVelocity(double velocity) override;

 protected:
  frc::sim::EncoderSim m_encoderSimulator;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
