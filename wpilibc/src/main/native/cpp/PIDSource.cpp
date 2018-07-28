/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/PIDSource.h"

using namespace frc;

void PIDSource::SetPIDSourceType(PIDSourceType pidSource) {
  m_pidSource = pidSource;
}

PIDSourceType PIDSource::GetPIDSourceType() const { return m_pidSource; }
