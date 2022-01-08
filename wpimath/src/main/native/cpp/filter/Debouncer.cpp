// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/filter/Debouncer.h"

using namespace frc;

Debouncer::Debouncer(units::second_t debounceTime, DebounceType type)
    : m_debounceTime(debounceTime), m_debounceType(type) {
  switch (type) {
    case DebounceType::kBoth:  // fall-through
    case DebounceType::kRising:
      m_baseline = false;
      break;
    case DebounceType::kFalling:
      m_baseline = true;
      break;
  }
  ResetTimer();
}

void Debouncer::ResetTimer() {
  m_prevTime = units::microsecond_t(wpi::Now());
}

bool Debouncer::HasElapsed() const {
  return units::microsecond_t(wpi::Now()) - m_prevTime >= m_debounceTime;
}

bool Debouncer::Calculate(bool input) {
  if (input == m_baseline) {
    ResetTimer();
  }

  if (HasElapsed()) {
    if (m_debounceType == DebounceType::kBoth) {
      m_baseline = input;
      ResetTimer();
    }
    return input;
  } else {
    return m_baseline;
  }
}
