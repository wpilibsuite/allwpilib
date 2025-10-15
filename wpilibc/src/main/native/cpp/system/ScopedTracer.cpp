// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ScopedTracer.h"

#include <wpi/raw_ostream.h>

using namespace frc;

ScopedTracer::ScopedTracer(std::string_view name, wpi::raw_ostream& os)
    : m_name(name), m_os(os) {
  m_tracer.ResetTimer();
}

ScopedTracer::~ScopedTracer() {
  m_tracer.AddEpoch(m_name);
  m_tracer.PrintEpochs(m_os);
}
