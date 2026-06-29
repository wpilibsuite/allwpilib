// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/Spectrum.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <numbers>
#include <vector>

#include "pocketfft_hdronly.h"

namespace wpi::filterdesigner {

std::optional<Spectrum> Spectrum::Compute(std::span<const double> samples,
                                          double sampleRate) {
  const size_t N = samples.size();
  if (N < 2 || sampleRate <= 0.0) {
    return std::nullopt;
  }

  // Periodic Hann (denominator N, not N-1) — matches scipy.signal.periodogram /
  // welch so golden values from SciPy line up.
  std::vector<double> windowed(N);
  double windowSum = 0.0;
  const double twoPiOverN = 2.0 * std::numbers::pi / static_cast<double>(N);
  for (size_t n = 0; n < N; ++n) {
    double w = 0.5 * (1.0 - std::cos(twoPiOverN * static_cast<double>(n)));
    windowed[n] = samples[n] * w;
    windowSum += w;
  }

  const size_t nBins = N / 2 + 1;
  std::vector<std::complex<double>> out(nBins);

  pocketfft::shape_t shape{N};
  pocketfft::stride_t strideIn{sizeof(double)};
  pocketfft::stride_t strideOut{sizeof(std::complex<double>)};
  pocketfft::r2c(shape, strideIn, strideOut, /*axis=*/0, /*forward=*/true,
                 windowed.data(), out.data(), /*fct=*/1.0);

  Spectrum spec;
  spec.frequencies.resize(nBins);
  spec.magnitudesDb.resize(nBins);
  const double invWindowSum = (windowSum > 0.0) ? 1.0 / windowSum : 1.0;
  constexpr double kEps = 1e-12;
  for (size_t k = 0; k < nBins; ++k) {
    spec.frequencies[k] =
        static_cast<double>(k) * sampleRate / static_cast<double>(N);
    double linear = std::abs(out[k]) * invWindowSum;
    // Double non-DC, non-Nyquist bins to account for folding to a single-sided
    // spectrum.
    const bool isNyquist = (N % 2 == 0) && (k == nBins - 1);
    if (k != 0 && !isNyquist) {
      linear *= 2.0;
    }
    spec.magnitudesDb[k] = 20.0 * std::log10(std::max(linear, kEps));
  }
  return spec;
}

}  // namespace wpi::filterdesigner
