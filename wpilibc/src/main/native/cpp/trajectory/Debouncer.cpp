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


