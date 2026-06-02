// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <span>
#include <vector>

namespace wpi::filterdesigner {

/**
 * Single-sided amplitude spectrum of a real signal, Hann-windowed.
 *
 * @a frequencies[k] is the bin frequency in Hz; @a magnitudesDb[k] is the
 * 20·log10 of the corresponding amplitude, normalized so that a coherent
 * sine of amplitude A shows up as ≈20·log10(A) dB at its frequency bin.
 */
struct Spectrum {
  std::vector<double> frequencies;
  std::vector<double> magnitudesDb;

  /**
   * Compute the Hann-windowed single-sided magnitude spectrum of @a samples.
   *
   * Returns @c std::nullopt when the signal is too short (< 2 samples) or the
   * sample rate is non-positive. FFT length is @a samples.size(); PocketFFT
   * handles non-power-of-two lengths but composite sizes are fastest.
   *
   * @param samples    Real-valued time series.
   * @param sampleRate Sampling frequency in Hz.
   */
  static std::optional<Spectrum> Compute(std::span<const double> samples,
                                         double sampleRate);
};

}  // namespace wpi::filterdesigner
