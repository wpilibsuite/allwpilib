// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/FrequencyPlotNodeLogic.hpp"

#include <algorithm>
#include <cmath>

namespace wpi::filterdesigner {

const Spectrum* FrequencyPlotNodeLogic::SpectrumFor(int slot,
                                                    const Signal* sig) {
  if (slot < 0 || slot >= kInputCount) {
    return nullptr;
  }
  CachedSpectrum& c = m_cache[slot];
  if (!sig) {
    c = {};
    return nullptr;
  }
  if (c.input == sig && c.revision == sig->revision &&
      c.sampleRate == sig->sampleRate && c.transient == sig->transient) {
    return c.spectrum ? &*c.spectrum : nullptr;
  }
  c.input = sig;
  c.revision = sig->revision;
  c.sampleRate = sig->sampleRate;
  c.transient = sig->transient;
  // Compute's refusals are indistinguishable to the caller, so record why.
  // A linear scan beside an FFT, and only on a miss.
  c.gaps = !std::ranges::all_of(sig->values,
                                [](double v) { return std::isfinite(v); });
  // Compute's own guards (rate, length, finiteness) decide whether there is a
  // spectrum; a signal that fails them is cached as "none" so it isn't
  // retried each frame either.
  c.spectrum = Spectrum::Compute(
      sig->values, sig->sampleRate,
      sig->transient ? SpectrumMode::kTransient : SpectrumMode::kStationary);
  ++m_computeCount;
  return c.spectrum ? &*c.spectrum : nullptr;
}

bool FrequencyPlotNodeLogic::HasGaps(int slot) const {
  if (slot < 0 || slot >= kInputCount) {
    return false;
  }
  return m_cache[slot].gaps;
}

bool FrequencyPlotNodeLogic::TakeXAxisRefit() {
  if (m_drawnLogFrequency == logFrequency) {
    return false;
  }
  m_drawnLogFrequency = logFrequency;
  return true;
}

}  // namespace wpi::filterdesigner
