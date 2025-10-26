// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/filter/TapCountFilter.h"
#include "wpimath/MathShared.h"

using namespace frc;

TapCountFilter::TapCountFilter(int requiredTaps, units::second_t tapWindow)
    : m_requiredTaps(requiredTaps), m_tapWindow(tapWindow) {
  ResetTimer();
}

void TapCountFilter::ResetTimer() {
  m_firstTapTime = wpi::math::MathSharedStore::GetTimestamp();
}

bool TapCountFilter::HasElapsed() const {
  return wpi::math::MathSharedStore::GetTimestamp() - m_firstTapTime >=
         m_tapWindow;
}

bool TapCountFilter::Calculate(bool input) {
  // Detect rising edge
  if (input && !m_lastInput) {
    if (m_currentTapCount == 0) {
      ResetTimer();  // Start timer on first rising edge
    }

    m_currentTapCount++;
  }

  bool activated = input && m_currentTapCount >= m_requiredTaps;

  bool timeExpired = HasElapsed() && !activated;
  bool activationEnded = !input && m_currentTapCount >= m_requiredTaps;

  if (timeExpired || activationEnded) {
    m_currentTapCount = 0;
  }

  m_lastInput = input;

  return activated;
}
