/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/ScopedTracer.h"

#include <wpi/raw_ostream.h>

using namespace frc;

ScopedTracer::ScopedTracer(wpi::Twine name, wpi::raw_ostream& os)
    : m_name(name.str()), m_os(os) {
  m_tracer.ResetTimer();
}

ScopedTracer::~ScopedTracer() {
  m_tracer.AddEpoch(m_name);
  m_tracer.PrintEpochs(m_os);
}
