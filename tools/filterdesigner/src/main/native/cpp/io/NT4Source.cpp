// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/io/NT4Source.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace wpi::filterdesigner {

NT4Source::NT4Source(DrainFn drain) : m_drain{std::move(drain)} {
  m_signal.live = true;
}

void NT4Source::Update() {
  bool changed = false;
  if (m_drain) {
    // Always drain, or a real subscriber's queue grows until unfreeze.
    // Discarding is what makes unfreeze mean "resume from now".
    auto newSamples = m_drain();
    if (!m_frozen && !newSamples.empty()) {
      if (!m_haveT0) {
        // Capture t0 from the first buffered sample, not the first drained
        // one, so freeze→drain-discarded samples don't pin the origin.
        m_t0Nanos = newSamples.front().timeNanos;
        m_haveT0 = true;
      }
      for (const auto& s : newSamples) {
        m_buffer.push_back(s);
      }
      changed = true;
    }
  }
  if (!m_frozen && !m_buffer.empty()) {
    // Nanoseconds per NT convention; SetBufferSeconds bounds the window to
    // kMaxBufferSeconds, well inside what int64_t nanoseconds can hold.
    int64_t windowNanos = static_cast<int64_t>(m_bufferSeconds * 1e9);
    int64_t cutoff = m_buffer.back().timeNanos - windowNanos;
    while (!m_buffer.empty() && m_buffer.front().timeNanos < cutoff) {
      m_buffer.pop_front();
      changed = true;
    }
  }

  if (changed) {
    Rebuild();
  }
}

void NT4Source::SetDiscrete(bool discrete) {
  if (m_signal.discrete == discrete) {
    return;
  }
  m_signal.discrete = discrete;
  // The type of a topic subscribed before the server announced it arrives
  // after its first samples were gridded, and a boolean that then holds
  // still would never trigger another Update rebuild.
  if (!m_buffer.empty()) {
    Rebuild();
  }
}

void NT4Source::Rebuild() {
  m_signal.timestamps.clear();
  m_signal.values.clear();
  m_signal.timestamps.reserve(m_buffer.size());
  m_signal.values.reserve(m_buffer.size());
  for (const auto& s : m_buffer) {
    m_signal.timestamps.push_back((s.timeNanos - m_t0Nanos) * 1e-9);
    m_signal.values.push_back(s.value);
  }
  // NT reports on change with network jitter on top, so the window is never
  // uniform as received; resample it before anyone assumes a fixed dt.
  m_signal.ResampleToGrid();
  ++m_signal.revision;
}

void NT4Source::SetBufferSeconds(double seconds) {
  if (std::isfinite(seconds) && seconds > 0.0) {
    m_bufferSeconds = std::min(seconds, kMaxBufferSeconds);
  }
}

void NT4Source::Clear() {
  m_buffer.clear();
  m_signal.timestamps.clear();
  m_signal.values.clear();
  m_signal.sampleRate = 0.0;
  m_signal.quality = GridQuality{};
  m_haveT0 = false;
  m_t0Nanos = 0;
  ++m_signal.revision;
}

}  // namespace wpi::filterdesigner
