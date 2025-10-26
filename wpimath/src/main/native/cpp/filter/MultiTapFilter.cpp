// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/filter/MultiTapFilter.h"

#include "wpimath/MathShared.h"

using namespace frc;

MultiTapFilter::MultiTapFilter(int requiredTaps, units::second_t tapWindow)
    : m_requiredTaps(requiredTaps), m_tapWindow(tapWindow) {
  ResetTimer();
}

void MultiTapFilter::ResetTimer() {
  m_firstTapTime = wpi::math::MathSharedStore::GetTimestamp();
}

bool MultiTapFilter::HasElapsed() const {
  return wpi::math::MathSharedStore::GetTimestamp() - m_firstTapTime >=
         m_tapWindow;
}

bool MultiTapFilter::Calculate(bool input) {
  bool enoughTaps = m_currentTapCount >= m_requiredTaps;

  bool expired = HasElapsed() && !enoughTaps;
  bool activationEnded = !input && enoughTaps;

  if (expired || activationEnded) {
    m_currentTapCount = 0;
  }

  if (input && !m_lastInput) {
    if (m_currentTapCount == 0) {
      ResetTimer();  // Start timer on first rising edge
    }

    m_currentTapCount++;
  }

  m_lastInput = input;

  return input && m_currentTapCount >= m_requiredTaps;
}
