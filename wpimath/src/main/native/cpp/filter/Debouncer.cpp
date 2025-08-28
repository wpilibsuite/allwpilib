// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/MathShared.h>
#include <wpi/math/filter/Debouncer.h>

using namespace wpi::math;

Debouncer::Debouncer(units::second_t debounceTime, DebounceType type)
    : m_debounceTime(debounceTime), m_debounceType(type) {
  m_baseline = m_debounceType == DebounceType::kFalling;
  ResetTimer();
}

void Debouncer::ResetTimer() {
  m_prevTime = wpi::math::MathSharedStore::GetTimestamp();
}

bool Debouncer::HasElapsed() const {
  return wpi::math::MathSharedStore::GetTimestamp() - m_prevTime >=
         m_debounceTime;
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
