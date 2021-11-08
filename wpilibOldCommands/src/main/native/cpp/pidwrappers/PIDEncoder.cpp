// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/pidwrappers/PIDEncoder.h"

using namespace frc;

double PIDEncoder::PIDGet() {
  switch (GetPIDSourceType()) {
    case PIDSourceType::kDisplacement:
      return GetDistance();
    case PIDSourceType::kRate:
      return GetRate();
    default:
      return 0.0;
  }
}
