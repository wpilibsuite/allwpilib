// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Debouncer.h"

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
  m_timer.Start();
}

bool Debouncer::Calculate(bool input) {
  if (input == m_baseline) {
    m_timer.Reset();
  }

  if (m_timer.HasElapsed(m_debounceTime)) {
    if (m_debounceType == DebounceType::kBoth) {
      m_baseline = input;
      m_timer.Reset();
    }
    return input;
  } else {
    return m_baseline;
  }
}
