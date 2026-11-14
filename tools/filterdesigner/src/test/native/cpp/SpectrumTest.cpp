// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/Spectrum.hpp"

#include <cmath>
#include <limits>
#include <numbers>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"

namespace {

using wpi::filterdesigner::Spectrum;
using wpi::filterdesigner::SpectrumMode;

TEST_CASE("SpectrumTest RejectsTooFewSamples", "[filterdesigner]") {
  std::vector<double> one{1.0};
  CHECK_FALSE(
      Spectrum::Compute(one, 1000.0, SpectrumMode::kStationary).has_value());
  std::vector<double> empty;
  CHECK_FALSE(
      Spectrum::Compute(empty, 1000.0, SpectrumMode::kStationary).has_value());
}

TEST_CASE("SpectrumTest RejectsNonPositiveSampleRate", "[filterdesigner]") {
  std::vector<double> s(16, 0.0);
  CHECK_FALSE(Spectrum::Compute(s, 0.0, SpectrumMode::kStationary).has_value());
  CHECK_FALSE(
      Spectrum::Compute(s, -1.0, SpectrumMode::kStationary).has_value());
}

TEST_CASE("SpectrumTest BinsCoverZeroThroughNyquist", "[filterdesigner]") {
  // N=8 -> 5 bins: 0, fs/8, fs/4, 3fs/8, fs/2
  std::vector<double> s(8, 1.0);
  auto spec = Spectrum::Compute(s, 800.0, SpectrumMode::kStationary);
  REQUIRE(spec.has_value());
  REQUIRE(spec->frequencies.size() == 5u);
  CHECK_DOUBLE_EQ(spec->frequencies[0], 0.0);
  CHECK_DOUBLE_EQ(spec->frequencies[1], 100.0);
  CHECK_DOUBLE_EQ(spec->frequencies[2], 200.0);
  CHECK_DOUBLE_EQ(spec->frequencies[3], 300.0);
  CHECK_DOUBLE_EQ(spec->frequencies[4], 400.0);
  CHECK(spec->magnitudesDb.size() == spec->frequencies.size());
}

TEST_CASE("SpectrumTest SinePeaksAtItsFrequencyWithUnitAmplitude",
          "[filterdesigner]") {
  // 50 Hz sine, amplitude 1, fs=1000, 1 s of data.
  constexpr int N = 1000;
  constexpr double fs = 1000.0;
  constexpr double f = 50.0;
  std::vector<double> s(N);
  for (int n = 0; n < N; ++n) {
    s[n] = std::sin(2.0 * std::numbers::pi * f * n / fs);
  }
  auto spec = Spectrum::Compute(s, fs, SpectrumMode::kStationary);
  REQUIRE(spec.has_value());

  // Find the peak and check it's close to 50 Hz.
  size_t peak = 0;
  for (size_t k = 1; k < spec->magnitudesDb.size(); ++k) {
    if (spec->magnitudesDb[k] > spec->magnitudesDb[peak]) {
      peak = k;
    }
  }
  CHECK_NEAR(spec->frequencies[peak], f, fs / N);
  // Amplitude 1.0 -> 0 dB; Hann window main lobe drops a touch.
  CHECK_NEAR(spec->magnitudesDb[peak], 0.0, 0.5);
}

TEST_CASE("SpectrumTest DcSignalRecoversAmplitudeAtBinZero",
          "[filterdesigner]") {
  // Constant signal of amplitude 1: DC bin recovers 0 dB, and bins past the
  // Hann main lobe (bin 1 leaks by design) sit at the silence floor. In
  // particular bin 0 must NOT be doubled for the single-sided fold.
  constexpr int N = 64;
  std::vector<double> s(N, 1.0);
  auto spec = Spectrum::Compute(s, 1000.0, SpectrumMode::kStationary);
  REQUIRE(spec.has_value());
  CHECK_NEAR(spec->magnitudesDb[0], 0.0, 0.1);
  for (size_t k = 2; k < spec->magnitudesDb.size(); ++k) {
    CHECK(spec->magnitudesDb[k] < -200.0);
  }
}

TEST_CASE("SpectrumTest NyquistBinNotDoubled", "[filterdesigner]") {
  // fs=1000, N=100 (even) -> Nyquist = 500 Hz, bin index 50.
  // Signal alternates +1/-1, i.e. a pure tone at fs/2 with amplitude 1.
  // A doubled Nyquist would read ~+6 dB instead of 0 dB.
  constexpr int N = 100;
  std::vector<double> s(N);
  for (int n = 0; n < N; ++n) {
    s[n] = (n % 2 == 0) ? 1.0 : -1.0;
  }
  auto spec = Spectrum::Compute(s, 1000.0, SpectrumMode::kStationary);
  REQUIRE(spec.has_value());
  const size_t nyquist = spec->frequencies.size() - 1;
  CHECK_DOUBLE_EQ(spec->frequencies[nyquist], 500.0);
  CHECK_NEAR(spec->magnitudesDb[nyquist], 0.0, 0.5);
}

TEST_CASE("SpectrumTest HalfAmplitudeSineReportsMinusSixDb",
          "[filterdesigner]") {
  // Verifies the single-sided doubling for non-DC / non-Nyquist bins:
  // amplitude 0.5 -> 20*log10(0.5) = -6.0206 dB.
  constexpr int N = 1000;
  constexpr double fs = 1000.0;
  constexpr double f = 50.0;
  constexpr double amp = 0.5;
  std::vector<double> s(N);
  for (int n = 0; n < N; ++n) {
    s[n] = amp * std::sin(2.0 * std::numbers::pi * f * n / fs);
  }
  auto spec = Spectrum::Compute(s, fs, SpectrumMode::kStationary);
  REQUIRE(spec.has_value());
  size_t peak = 0;
  for (size_t k = 1; k < spec->magnitudesDb.size(); ++k) {
    if (spec->magnitudesDb[k] > spec->magnitudesDb[peak]) {
      peak = k;
    }
  }
  CHECK_NEAR(spec->magnitudesDb[peak], 20.0 * std::log10(amp), 0.5);
}

TEST_CASE("SpectrumTest SilenceFloorsToDbEpsilon", "[filterdesigner]") {
  std::vector<double> s(64, 0.0);
  auto spec = Spectrum::Compute(s, 1000.0, SpectrumMode::kStationary);
  REQUIRE(spec.has_value());
  // log10(1e-12) = -12, times 20 = -240. Everything should hit the floor.
  for (double db : spec->magnitudesDb) {
    CHECK(db < -200.0);
  }
}

TEST_CASE("SpectrumTest StationaryModeWindowsOutALeadingImpulse",
          "[filterdesigner]") {
  // The periodic Hann window is exactly zero at index 0, so the stationary
  // transform sees a unit impulse there as silence. This is why transients
  // get a mode of their own.
  std::vector<double> s(64, 0.0);
  s[0] = 1.0;
  auto spec = Spectrum::Compute(s, 1000.0, SpectrumMode::kStationary);
  REQUIRE(spec.has_value());
  for (double db : spec->magnitudesDb) {
    CHECK(db < -200.0);
  }
}

TEST_CASE("SpectrumTest TransientModeReadsAUnitImpulseAsZeroDbEverywhere",
          "[filterdesigner]") {
  std::vector<double> s(64, 0.0);
  s[0] = 1.0;
  auto spec = Spectrum::Compute(s, 1000.0, SpectrumMode::kTransient);
  REQUIRE(spec.has_value());
  REQUIRE(spec->magnitudesDb.size() == 33u);
  for (double db : spec->magnitudesDb) {
    CHECK_NEAR(db, 0.0, 1e-9);
  }
}

TEST_CASE("SpectrumTest TransientModeAppliesNoWindowAtAll",
          "[filterdesigner]") {
  // A delayed impulse has the same flat magnitude as one at index 0; any
  // taper would attenuate it by the window's value at that index.
  std::vector<double> s(64, 0.0);
  s[5] = 1.0;
  auto spec = Spectrum::Compute(s, 1000.0, SpectrumMode::kTransient);
  REQUIRE(spec.has_value());
  for (double db : spec->magnitudesDb) {
    CHECK_NEAR(db, 0.0, 1e-9);
  }
}

TEST_CASE("SpectrumTest TransientModeReadsAFilteredImpulseAsTheResponse",
          "[filterdesigner]") {
  // Impulse response of a two-tap moving average, h = {0.5, 0.5}: |H(f)| =
  // |cos(pi f / fs)|, so 0 dB at DC, -3.01 dB at fs/4 and a null at Nyquist.
  // fs=1000, N=64 -> bin 16 is 250 Hz, bin 32 is 500 Hz.
  std::vector<double> s(64, 0.0);
  s[0] = 0.5;
  s[1] = 0.5;
  auto spec = Spectrum::Compute(s, 1000.0, SpectrumMode::kTransient);
  REQUIRE(spec.has_value());
  CHECK_NEAR(spec->magnitudesDb[0], 0.0, 1e-9);
  CHECK_NEAR(spec->magnitudesDb[16], 20.0 * std::log10(std::sqrt(0.5)), 1e-9);
  CHECK(spec->magnitudesDb[32] < -200.0);
}

TEST_CASE("SpectrumTest NonFiniteSampleHasNoSpectrum", "[filterdesigner]") {
  // One bad reading is not a bad bin, it is the whole spectrum gone.
  std::vector<double> samples(64, 1.0);
  samples[10] = std::numeric_limits<double>::quiet_NaN();
  CHECK_FALSE(Spectrum::Compute(samples, 1000.0, SpectrumMode::kStationary)
                  .has_value());

  samples[10] = std::numeric_limits<double>::infinity();
  CHECK_FALSE(Spectrum::Compute(samples, 1000.0, SpectrumMode::kStationary)
                  .has_value());

  samples[10] = -std::numeric_limits<double>::infinity();
  CHECK_FALSE(Spectrum::Compute(samples, 1000.0, SpectrumMode::kStationary)
                  .has_value());
}

TEST_CASE("SpectrumTest NonFiniteSampleHasNoTransientSpectrumEither",
          "[filterdesigner]") {
  std::vector<double> samples(64, 0.0);
  samples[0] = 1.0;
  samples[20] = std::numeric_limits<double>::quiet_NaN();
  CHECK_FALSE(
      Spectrum::Compute(samples, 1000.0, SpectrumMode::kTransient).has_value());
}

}  // namespace
