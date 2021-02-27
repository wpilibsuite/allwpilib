// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/interfaces/Potentiometer.h"

#include "frc/Utility.h"

using namespace frc;

void Potentiometer::SetPIDSourceType(PIDSourceType pidSource) {
  if (wpi_assert(pidSource == PIDSourceType::kDisplacement)) {
    m_pidSource = pidSource;
  }
}
