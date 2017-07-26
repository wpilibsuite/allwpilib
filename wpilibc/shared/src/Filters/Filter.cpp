/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Filters/Filter.h"

frc::Filter::Filter(std::shared_ptr<PIDSource> source) { m_source = source; }

void frc::Filter::SetPIDSourceType(PIDSourceType pidSource) {
  m_source->SetPIDSourceType(pidSource);
}

frc::PIDSourceType frc::Filter::GetPIDSourceType() const {
  return m_source->GetPIDSourceType();
}

double frc::Filter::PIDGetSource() { return m_source->PIDGet(); }
