// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/ImpulseNodeLogic.hpp"

#include <algorithm>
#include <cstddef>

namespace wpi::filterdesigner {

const wpi::filterdesigner::Signal* ImpulseNodeLogic::Signal() const {
  if (sampleRate <= 0.0 || length < kMinLength) {
    m_signal.reset();
    m_lastSampleRate = 0.0;
    m_lastLength = 0;
    return nullptr;
  }
  int clampedLen = std::min(length, kMaxLength);
  if (m_signal.has_value() && m_lastSampleRate == sampleRate &&
      m_lastLength == clampedLen) {
    return &*m_signal;
  }

  wpi::filterdesigner::Signal s;
  s.name = "impulse";
  s.sampleRate = sampleRate;
  s.uniform = true;
  s.timestamps.resize(static_cast<std::size_t>(clampedLen));
  s.values.assign(static_cast<std::size_t>(clampedLen), 0.0);
  for (int i = 0; i < clampedLen; ++i) {
    s.timestamps[static_cast<std::size_t>(i)] =
        static_cast<double>(i) / sampleRate;
  }
  s.values[0] = 1.0;
  s.revision = ++m_revision;
  m_signal = std::move(s);
  m_lastSampleRate = sampleRate;
  m_lastLength = clampedLen;
  return &*m_signal;
}

}  // namespace wpi::filterdesigner
