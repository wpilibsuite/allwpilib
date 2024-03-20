// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <units/length.h>

#include "frc/SharpIR.h"

namespace frc {

class SharpIRSim {
 public:
  explicit SharpIRSim(const SharpIR& sharpIR);
  explicit SharpIRSim(int channel);

  void SetRange(units::centimeter_t rng);

 private:
  hal::SimDouble m_simRange;
};

}  // namespace frc
