/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "interfaces/Potentiometer.h"

#include <Utility.h>

using namespace frc;

void Potentiometer::SetPIDSourceType(PIDSourceType pidSource) {
  if (wpi_assert(pidSource == PIDSourceType::kDisplacement)) {
    m_pidSource = pidSource;
  }
}
