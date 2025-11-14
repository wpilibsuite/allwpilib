// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/filter/EdgeCountFilter.h"

#include "wpimath/MathShared.h"

using namespace frc;

EdgeCounterFilter::EdgeCounterFilter(int requiredEdges, units::second_t window)
    : m_requiredEdges(requiredEdges), m_windowTime(window) {
  ResetTimer();
}

void EdgeCounterFilter::ResetTimer() {
  m_firstEdgeTime = wpi::math::MathSharedStore::GetTimestamp();
}

bool EdgeCounterFilter::HasElapsed() const {
  return wpi::math::MathSharedStore::GetTimestamp() - m_firstEdgeTime >=
         m_windowTime;
}

bool EdgeCounterFilter::Calculate(bool input) {
  bool enoughEdges = m_currentCount >= m_requiredEdges;

  bool expired = HasElapsed() && !enoughEdges;
  bool activationEnded = !input && enoughEdges;

  if (expired || activationEnded) {
    m_currentCount = 0;
  }

  if (input && !m_lastInput) {
    if (m_currentCount == 0) {
      ResetTimer();  // Start timer on first rising edge
    }

    m_currentCount++;
  }

  m_lastInput = input;

  return input && m_currentCount >= m_requiredEdges;
}
