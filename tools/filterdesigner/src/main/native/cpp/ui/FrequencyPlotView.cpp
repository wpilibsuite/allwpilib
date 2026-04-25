// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/ui/FrequencyPlotView.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <algorithm>
#include <limits>
#include <utility>

#include <imgui.h>
#include <implot.h>

namespace wpi::filterdesigner {

namespace {
// Skip this many seconds of filter startup transient before FFT'ing the
// filtered signal, so the low-frequency transient doesn't masquerade as real
// passband content.
constexpr double kFilterTransientSeconds = 0.2;
}  // namespace

void FrequencyPlotView::Display(const Signal* signal,
                                std::span<const double> filtered,
                                uint64_t filterVersion) {
  if (!signal) {
    ImGui::TextDisabled("Pick a numeric entry from Data Source.");
    m_haveSpectrum = false;
    m_haveFilteredSpectrum = false;
    m_lastSignal = nullptr;
    return;
  }

  if (signal != m_lastSignal || signal->values.size() != m_lastSampleCount ||
      signal->sampleRate != m_lastSampleRate || signal->name != m_lastName) {
    auto spec = Spectrum::Compute(signal->values, signal->sampleRate);
    m_haveSpectrum = spec.has_value();
    if (m_haveSpectrum) {
      m_spectrum = std::move(*spec);
    }
    m_lastSignal = signal;
    m_lastSampleCount = signal->values.size();
    m_lastSampleRate = signal->sampleRate;
    m_lastName = signal->name;
    // Force the filtered spectrum to re-sync on the next branch.
    m_haveFilteredSpectrum = false;
    m_lastFilterVersion = 0;
    m_lastFilteredSize = 0;
  }

  bool filteredChanged = filtered.size() != m_lastFilteredSize ||
                         filterVersion != m_lastFilterVersion;
  if (filteredChanged) {
    m_haveFilteredSpectrum = false;
    if (!filtered.empty() && filtered.size() == signal->values.size() &&
        signal->sampleRate > 0.0) {
      size_t drop =
          static_cast<size_t>(signal->sampleRate * kFilterTransientSeconds);
      drop = std::min(drop, filtered.size() / 4);
      if (filtered.size() > drop + 2) {
        auto spec =
            Spectrum::Compute(filtered.subspan(drop), signal->sampleRate);
        if (spec.has_value()) {
          m_filteredSpectrum = std::move(*spec);
          m_haveFilteredSpectrum = true;
        }
      }
    }
    m_lastFilterVersion = filterVersion;
    m_lastFilteredSize = filtered.size();
  }

  if (signal->sampleRate <= 0.0) {
    ImGui::TextDisabled(
        "Sample rate unknown — cannot compute frequency spectrum.");
    return;
  }
  if (!m_haveSpectrum) {
    ImGui::TextDisabled("Signal too short for FFT.");
    return;
  }

  ImGui::Text("FFT length: %zu  |  Nyquist: %.1f Hz", signal->values.size(),
              signal->sampleRate * 0.5);

  if (ImPlot::BeginPlot("##freqplot", ImVec2{-1, -1})) {
    ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
    ImPlot::SetupAxis(ImAxis_Y1, "Magnitude (dB)");
    ImPlot::SetupLegend(ImPlotLocation_NorthEast);
    int count = static_cast<int>(std::min<size_t>(
        m_spectrum.frequencies.size(), std::numeric_limits<int>::max()));
    ImPlot::PlotLine("Raw", m_spectrum.frequencies.data(),
                     m_spectrum.magnitudesDb.data(), count);
    if (m_haveFilteredSpectrum) {
      int fcount = static_cast<int>(
          std::min<size_t>(m_filteredSpectrum.frequencies.size(),
                           std::numeric_limits<int>::max()));
      ImPlot::PlotLine("Filtered", m_filteredSpectrum.frequencies.data(),
                       m_filteredSpectrum.magnitudesDb.data(), fcount);
    }
    ImPlot::EndPlot();
  }
}

}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS
