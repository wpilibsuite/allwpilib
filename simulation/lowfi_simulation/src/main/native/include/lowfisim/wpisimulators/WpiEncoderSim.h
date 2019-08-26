/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "lowfisim/EncoderSim.h"
#include "simulation/EncoderSim.h"

namespace frc {
namespace sim {
namespace lowfi {

class WpiEncoderSim : public EncoderSim {
 public:
  explicit WpiEncoderSim(int index);
  bool IsWrapperInitialized() const override;

  void SetPosition(double position) override;
  void SetVelocity(double velocity) override;

 protected:
  frc::sim::EncoderSim m_encoderSimulator;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
