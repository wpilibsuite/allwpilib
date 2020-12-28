// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/filters/Filter.h"

#include "frc/Base.h"

using namespace frc;

Filter::Filter(PIDSource& source)
    : m_source(std::shared_ptr<PIDSource>(&source, NullDeleter<PIDSource>())) {}

Filter::Filter(std::shared_ptr<PIDSource> source)
    : m_source(std::move(source)) {}

void Filter::SetPIDSourceType(PIDSourceType pidSource) {
  m_source->SetPIDSourceType(pidSource);
}

PIDSourceType Filter::GetPIDSourceType() const {
  return m_source->GetPIDSourceType();
}

double Filter::PIDGetSource() {
  return m_source->PIDGet();
}
