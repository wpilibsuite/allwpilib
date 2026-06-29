// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/io/NT4Source.hpp"

#include <utility>

namespace wpi::filterdesigner {

NT4Source::NT4Source(DrainFn drain) : m_drain{std::move(drain)} {
  m_signal.live = true;
}

void NT4Source::Update() {
  bool changed = false;
  if (m_drain) {
    // Always drain — if we skipped while frozen, a real subscriber's queue
    // would grow unboundedly until unfreeze. Discard instead of buffering
    // so "unfreeze" means "resume from now" rather than "dump whatever
    // piled up."
    auto newSamples = m_drain();
    if (!m_frozen && !newSamples.empty()) {
      if (!m_haveT0) {
        // Capture t0 from the first buffered sample, not the first drained
        // one, so freeze→drain-discarded samples don't pin the origin.
        m_t0Micros = newSamples.front().timeMicros;
        m_haveT0 = true;
      }
      for (const auto& s : newSamples) {
        m_buffer.push_back(s);
      }
      changed = true;
    }
  }
  if (!m_frozen && !m_buffer.empty()) {
    // Microseconds per NT convention; SetBufferSeconds guards against
    // overflow for any reasonable window length.
    int64_t windowMicros = static_cast<int64_t>(m_bufferSeconds * 1e6);
    int64_t cutoff = m_buffer.back().timeMicros - windowMicros;
    while (!m_buffer.empty() && m_buffer.front().timeMicros < cutoff) {
      m_buffer.pop_front();
      changed = true;
    }
  }

  if (!changed) {
    return;
  }

  m_signal.timestamps.clear();
  m_signal.values.clear();
  m_signal.timestamps.reserve(m_buffer.size());
  m_signal.values.reserve(m_buffer.size());
  for (const auto& s : m_buffer) {
    m_signal.timestamps.push_back((s.timeMicros - m_t0Micros) * 1e-6);
    m_signal.values.push_back(s.value);
  }
  m_signal.sampleRate = Signal::InferSampleRate(m_signal.timestamps);
  m_signal.uniform = Signal::IsUniform(m_signal.timestamps);
  ++m_signal.revision;
}

void NT4Source::SetBufferSeconds(double seconds) {
  if (seconds > 0.0) {
    m_bufferSeconds = seconds;
  }
}

void NT4Source::Clear() {
  m_buffer.clear();
  m_signal.timestamps.clear();
  m_signal.values.clear();
  m_signal.sampleRate = 0.0;
  m_signal.uniform = false;
  m_haveT0 = false;
  m_t0Micros = 0;
  ++m_signal.revision;
}

}  // namespace wpi::filterdesigner
