// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/Spectrum.hpp"

#include <cmath>
#include <numbers>
#include <vector>

#include <gtest/gtest.h>

namespace {

using wpi::filterdesigner::Spectrum;

TEST(SpectrumTest, RejectsTooFewSamples) {
  std::vector<double> one{1.0};
  EXPECT_FALSE(Spectrum::Compute(one, 1000.0).has_value());
  std::vector<double> empty;
  EXPECT_FALSE(Spectrum::Compute(empty, 1000.0).has_value());
}

TEST(SpectrumTest, RejectsNonPositiveSampleRate) {
  std::vector<double> s(16, 0.0);
  EXPECT_FALSE(Spectrum::Compute(s, 0.0).has_value());
  EXPECT_FALSE(Spectrum::Compute(s, -1.0).has_value());
}

TEST(SpectrumTest, BinsCoverZeroThroughNyquist) {
  // N=8 -> 5 bins: 0, fs/8, fs/4, 3fs/8, fs/2
  std::vector<double> s(8, 1.0);
  auto spec = Spectrum::Compute(s, 800.0);
  ASSERT_TRUE(spec.has_value());
  ASSERT_EQ(spec->frequencies.size(), 5u);
  EXPECT_DOUBLE_EQ(spec->frequencies[0], 0.0);
  EXPECT_DOUBLE_EQ(spec->frequencies[1], 100.0);
  EXPECT_DOUBLE_EQ(spec->frequencies[2], 200.0);
  EXPECT_DOUBLE_EQ(spec->frequencies[3], 300.0);
  EXPECT_DOUBLE_EQ(spec->frequencies[4], 400.0);
  EXPECT_EQ(spec->magnitudesDb.size(), spec->frequencies.size());
}

TEST(SpectrumTest, SinePeaksAtItsFrequencyWithUnitAmplitude) {
  // 50 Hz sine, amplitude 1, fs=1000, 1 s of data.
  constexpr int N = 1000;
  constexpr double fs = 1000.0;
  constexpr double f = 50.0;
  std::vector<double> s(N);
  for (int n = 0; n < N; ++n) {
    s[n] = std::sin(2.0 * std::numbers::pi * f * n / fs);
  }
  auto spec = Spectrum::Compute(s, fs);
  ASSERT_TRUE(spec.has_value());

  // Find the peak and check it's close to 50 Hz.
  size_t peak = 0;
  for (size_t k = 1; k < spec->magnitudesDb.size(); ++k) {
    if (spec->magnitudesDb[k] > spec->magnitudesDb[peak]) {
      peak = k;
    }
  }
  EXPECT_NEAR(spec->frequencies[peak], f, fs / N);
  // Amplitude 1.0 -> 0 dB; Hann window main lobe drops a touch.
  EXPECT_NEAR(spec->magnitudesDb[peak], 0.0, 0.5);
}

TEST(SpectrumTest, DcSignalRecoversAmplitudeAtBinZero) {
  // Constant signal of amplitude 1: DC bin recovers 0 dB, and bins past the
  // Hann main lobe (bin 1 leaks by design) sit at the silence floor. In
  // particular bin 0 must NOT be doubled for the single-sided fold.
  constexpr int N = 64;
  std::vector<double> s(N, 1.0);
  auto spec = Spectrum::Compute(s, 1000.0);
  ASSERT_TRUE(spec.has_value());
  EXPECT_NEAR(spec->magnitudesDb[0], 0.0, 0.1);
  for (size_t k = 2; k < spec->magnitudesDb.size(); ++k) {
    EXPECT_LT(spec->magnitudesDb[k], -200.0);
  }
}

TEST(SpectrumTest, NyquistBinNotDoubled) {
  // fs=1000, N=100 (even) -> Nyquist = 500 Hz, bin index 50.
  // Signal alternates +1/-1, i.e. a pure tone at fs/2 with amplitude 1.
  // A doubled Nyquist would read ~+6 dB instead of 0 dB.
  constexpr int N = 100;
  std::vector<double> s(N);
  for (int n = 0; n < N; ++n) {
    s[n] = (n % 2 == 0) ? 1.0 : -1.0;
  }
  auto spec = Spectrum::Compute(s, 1000.0);
  ASSERT_TRUE(spec.has_value());
  const size_t nyquist = spec->frequencies.size() - 1;
  EXPECT_DOUBLE_EQ(spec->frequencies[nyquist], 500.0);
  EXPECT_NEAR(spec->magnitudesDb[nyquist], 0.0, 0.5);
}

TEST(SpectrumTest, HalfAmplitudeSineReportsMinusSixDb) {
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
  auto spec = Spectrum::Compute(s, fs);
  ASSERT_TRUE(spec.has_value());
  size_t peak = 0;
  for (size_t k = 1; k < spec->magnitudesDb.size(); ++k) {
    if (spec->magnitudesDb[k] > spec->magnitudesDb[peak]) {
      peak = k;
    }
  }
  EXPECT_NEAR(spec->magnitudesDb[peak], 20.0 * std::log10(amp), 0.5);
}

TEST(SpectrumTest, SilenceFloorsToDbEpsilon) {
  std::vector<double> s(64, 0.0);
  auto spec = Spectrum::Compute(s, 1000.0);
  ASSERT_TRUE(spec.has_value());
  // log10(1e-12) = -12, times 20 = -240. Everything should hit the floor.
  for (double db : spec->magnitudesDb) {
    EXPECT_LT(db, -200.0);
  }
}

}  // namespace
