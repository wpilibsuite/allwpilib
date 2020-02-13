/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Debouncer.h"

using namespace frc;

Debouncer::Debouncer(units::second_t debounceTime, bool baseline)
    : m_debounceTime(debounceTime), m_baseline(baseline) {
  m_timer.Start();
}

bool Debouncer::Calculate(bool input) {
  if (input == m_baseline) {
    m_timer.Reset();
  }

  if (m_timer.HasElapsed(m_debounceTime)) {
    return !m_baseline;
  } else {
    return m_baseline;
  }
}
