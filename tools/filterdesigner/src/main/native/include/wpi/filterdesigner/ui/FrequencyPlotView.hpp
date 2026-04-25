// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <span>
#include <string>

#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/filterdesigner/model/Spectrum.hpp"

namespace wpi::filterdesigner {

/**
 * ImGui view that renders the Hann-windowed log-magnitude spectrum of a
 * @ref Signal, with an optional second spectrum (typically the signal run
 * through the current filter design). Both spectra are cached and only
 * recomputed when their inputs change.
 */
class FrequencyPlotView {
 public:
  /**
   * Draw the plot.
   *
   * @param signal         Raw signal; may be null.
   * @param filtered       Filtered samples aligned with @a signal->values.
   *                       Empty hides the overlay.
   * @param filterVersion  Incremented whenever @a filtered changes content,
   *                       used as a cache key so the FFT is not recomputed on
   *                       every frame.
   */
  void Display(const Signal* signal, std::span<const double> filtered = {},
               uint64_t filterVersion = 0);

 private:
  const Signal* m_lastSignal = nullptr;
  uint64_t m_lastRevision = 0;
  double m_lastSampleRate = 0.0;
  std::string m_lastName;
  Spectrum m_spectrum;
  bool m_haveSpectrum = false;

  Spectrum m_filteredSpectrum;
  bool m_haveFilteredSpectrum = false;
  uint64_t m_lastFilterVersion = 0;
  size_t m_lastFilteredSize = 0;
};

}  // namespace wpi::filterdesigner
