// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <span>
#include <vector>

namespace wpi::filterdesigner {

/** Which transform @ref Spectrum::Compute runs, chosen by what the signal is.
 */
enum class SpectrumMode {
  /**
   * An ongoing signal (a logged sensor, a tone): Hann-windowed amplitude
   * spectrum, normalized so a coherent sine of amplitude A reads
   * ≈20·log10(A) dB at its bin.
   */
  kStationary,
  /**
   * A one-off event (an impulse or step, filtered or not): the raw DFT
   * magnitude, with no window and no normalization, so a unit impulse reads
   * 0 dB at every bin and a filtered impulse reads the filter's frequency
   * response. A window would suppress the very samples that carry the event;
   * the Hann window is exactly zero at index 0, where an impulse sits.
   */
  kTransient,
};

/**
 * Single-sided magnitude spectrum of a real signal.
 *
 * @a frequencies[k] is the bin frequency in Hz; @a magnitudesDb[k] is
 * 20·log10 of the corresponding magnitude, scaled as @ref SpectrumMode
 * describes.
 */
struct Spectrum {
  std::vector<double> frequencies;
  std::vector<double> magnitudesDb;

  /**
   * Compute the single-sided magnitude spectrum of @a samples.
   *
   * Returns @c std::nullopt when the signal is too short (< 2 samples), the
   * sample rate is non-positive, or any sample is non-finite. FFT length is
   * @a samples.size(); PocketFFT handles non-power-of-two lengths but
   * composite sizes are fastest.
   *
   * @param samples    Real-valued time series.
   * @param sampleRate Sampling frequency in Hz.
   * @param mode       Windowing and scaling, per what @a samples holds.
   */
  static std::optional<Spectrum> Compute(std::span<const double> samples,
                                         double sampleRate, SpectrumMode mode);
};

}  // namespace wpi::filterdesigner
