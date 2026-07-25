// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/filter/BiquadFilter.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/math/filter/LinearFilter.hpp"
#include "wpi/units/time.hpp"

using wpi::math::BiquadFilter;

TEST(BiquadFilterTest, PassThrough) {
  BiquadFilter filter({{1.0, 0.0, 0.0, 0.0, 0.0}});

  std::mt19937 rng(42);
  std::uniform_real_distribution<double> dist(-100.0, 100.0);
  for (int i = 0; i < 200; ++i) {
    double x = dist(rng);
    EXPECT_DOUBLE_EQ(filter.Calculate(x), x);
  }
}

TEST(BiquadFilterTest, FirstOrderMatchesSinglePoleIIR) {
  // SinglePoleIIR: y[n] = (1-g) x[n] + g y[n-1], g = exp(-dt/T)
  // As biquad:     {1-g, 0, 0, -g, 0}
  constexpr double kTimeConstant = 0.015915;
  constexpr double kPeriod = 0.005;
  double g = std::exp(-kPeriod / kTimeConstant);

  auto linear =
      wpi::math::LinearFilter<double>::SinglePoleIIR(kTimeConstant, 5_ms);
  BiquadFilter biquad({{1.0 - g, 0.0, 0.0, -g, 0.0}});

  std::mt19937 rng(7);
  std::uniform_real_distribution<double> dist(-50.0, 50.0);
  for (int i = 0; i < 1000; ++i) {
    double x = dist(rng);
    double y_lin = linear.Calculate(x);
    double y_biq = biquad.Calculate(x);
    EXPECT_NEAR(y_lin, y_biq, 1e-12);
  }
}

TEST(BiquadFilterTest, Butterworth4thOrderLowPass) {
  // scipy.signal.butter(4, 50.0, btype='low', fs=1000.0, output='sos')
  BiquadFilter filter({
      {0.00041659920440659937, 0.0008331984088131987, 0.00041659920440659937,
       -1.4796742169311934, 0.5558215432824889},
      {1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979},
  });

  // Impulse response, first 30 samples, from scipy.signal.sosfilt.
  constexpr std::array<double, 30> kExpected = {
      0.00041659920440659937, 0.0029914483065925663,  0.010405740533503665,
      0.024092655231875183,   0.04300386328531425,    0.06442081415630327,
      0.08518000836484753,    0.10245740377665029,    0.1142030744642985,
      0.11931076610150239,    0.11759868177262096,    0.10966797135549569,
      0.09669445862739445,    0.08019770689053446,    0.06182021082200691,
      0.04313888252371942,    0.025521549440937964,   0.01003324447867498,
      -0.002609160074363505,  -0.01203989315971688,   -0.018213508615082776,
      -0.021350392922805498,  -0.02186860712506684,   -0.020311212598085788,
      -0.01727668431352673,   -0.013358111475758645,  -0.009094876704322833,
      -0.004938595712161598,  -0.0012334395430879353, 0.0017903545884787877,
  };

  for (size_t i = 0; i < kExpected.size(); ++i) {
    double x = (i == 0) ? 1.0 : 0.0;
    double y = filter.Calculate(x);
    EXPECT_NEAR(y, kExpected[i], 1e-10) << "sample " << i;
  }
}

TEST(BiquadFilterTest, Notch60Hz) {
  // scipy.signal.iirnotch(60.0, Q=10.0, fs=1000.0), converted via tf2sos
  BiquadFilter filter({
      {0.9814970254751076, -1.8251457105120343, 0.9814970254751076,
       -1.8251457105120341, 0.9629940509502151},
  });

  constexpr double kFs = 1000.0;
  constexpr int kSamples = 1000;
  std::vector<double> output(kSamples);
  for (int n = 0; n < kSamples; ++n) {
    double t = n / kFs;
    double x = std::sin(2.0 * std::numbers::pi * 10.0 * t) +
               std::sin(2.0 * std::numbers::pi * 60.0 * t);
    output[n] = filter.Calculate(x);
  }

  // Spot-check against scipy.signal.sosfilt outputs
  EXPECT_NEAR(output[500], -0.017355123579818322, 1e-10);
  EXPECT_NEAR(output[999], -0.08007594066581347, 1e-10);

  // Attenuation check via a basic DFT at 10 Hz and 60 Hz over the last 512
  // samples (in steady state). 60 Hz should be strongly attenuated, 10 Hz
  // should pass almost untouched.
  constexpr int kWindow = 512;
  auto bin = [&](const std::vector<double>& sig, double freq) {
    double re = 0.0;
    double im = 0.0;
    for (int n = 0; n < kWindow; ++n) {
      double x = sig[kSamples - kWindow + n];
      double phase = 2.0 * std::numbers::pi * freq * n / kFs;
      re += x * std::cos(phase);
      im -= x * std::sin(phase);
    }
    return std::hypot(re, im);
  };

  std::vector<double> input(kSamples);
  for (int n = 0; n < kSamples; ++n) {
    double t = n / kFs;
    input[n] = std::sin(2.0 * std::numbers::pi * 10.0 * t) +
               std::sin(2.0 * std::numbers::pi * 60.0 * t);
  }

  double in10 = bin(input, 10.0);
  double in60 = bin(input, 60.0);
  double out10 = bin(output, 10.0);
  double out60 = bin(output, 60.0);

  double atten60_dB = 20.0 * std::log10(out60 / in60);
  double atten10_dB = 20.0 * std::log10(out10 / in10);

  EXPECT_LT(atten60_dB, -40.0) << "60 Hz not sufficiently attenuated";
  EXPECT_GT(atten10_dB, -0.5) << "10 Hz passband loss too large";
}

TEST(BiquadFilterTest, Order8ButterworthMatchesScipy) {
  // High-order filter = 4 biquads. This test exists to prove that the SOS
  // (Direct Form II Transposed) implementation is numerically correct at the
  // orders that a flattened-polynomial LinearFilter cannot reliably run.
  //
  // scipy.signal.butter(8, 100.0, btype='low', fs=1000.0, output='sos')
  BiquadFilter filter({
      {2.3959644103776166e-05, 4.791928820755233e-05, 2.3959644103776166e-05,
       -1.0263514742610553, 0.26864019099379005},
      {1.0, 2.0, 1.0, -1.0868584613628944, 0.343430940165366},
      {1.0, 2.0, 1.0, -1.2197253651240232, 0.5076634651740437},
      {1.0, 2.0, 1.0, -1.4515795942478362, 0.794251053241888},
  });

  // Linear chirp from 1 Hz to 200 Hz over 500 samples at 1 kHz.
  // Matches scipy.signal.chirp(t, f0=1, f1=200, t1=t[-1], method='linear').
  constexpr int kSamples = 500;
  constexpr double kFs = 1000.0;
  constexpr double kF0 = 1.0;
  constexpr double kF1 = 200.0;
  const double t1 = (kSamples - 1) / kFs;
  const double k = (kF1 - kF0) / t1;

  std::array<double, 5> spot_samples{};
  constexpr std::array<int, 5> kSpotIndices{10, 50, 100, 250, 499};
  constexpr std::array<double, 5> kExpected{
      0.8950675041062186,  -0.7902247252134351,    0.1716891991372734,
      0.05240058121316523, -0.0016952227415119995,
  };

  size_t spot_idx = 0;
  for (int n = 0; n < kSamples; ++n) {
    double t = n / kFs;
    double phase = 2.0 * std::numbers::pi * (kF0 * t + 0.5 * k * t * t);
    double x = std::cos(phase);
    double y = filter.Calculate(x);

    if (spot_idx < kSpotIndices.size() && n == kSpotIndices[spot_idx]) {
      spot_samples[spot_idx++] = y;
    }
  }

  for (size_t i = 0; i < kExpected.size(); ++i) {
    EXPECT_NEAR(spot_samples[i], kExpected[i], 1e-10)
        << "sample index " << kSpotIndices[i];
  }
}

TEST(BiquadFilterTest, ResetZerosState) {
  BiquadFilter filter({
      {0.00041659920440659937, 0.0008331984088131987, 0.00041659920440659937,
       -1.4796742169311934, 0.5558215432824889},
      {1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979},
  });

  for (int i = 0; i < 50; ++i) {
    filter.Calculate(1.0);
  }
  EXPECT_NE(filter.LastValue(), 0.0);

  filter.Reset();
  EXPECT_DOUBLE_EQ(filter.LastValue(), 0.0);

  // First call after Reset should behave like the filter starts fresh —
  // matches the impulse-response first sample.
  double y = filter.Calculate(1.0);
  EXPECT_NEAR(y, 0.00041659920440659937, 1e-12);
}

TEST(BiquadFilterTest, ResetToSteadyState) {
  // DC gain of each section is (b0+b1+b2)/(1+a1+a2). After Reset(value),
  // Calculate(value) should immediately return value * cascade_DC_gain.
  BiquadFilter filter({
      {0.00041659920440659937, 0.0008331984088131987, 0.00041659920440659937,
       -1.4796742169311934, 0.5558215432824889},
      {1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979},
  });

  constexpr double kInput = 3.0;
  filter.Reset(kInput);

  // Cascade DC gain for a Butterworth LP is 1.0, so output should equal input.
  EXPECT_NEAR(filter.LastValue(), kInput, 1e-12);
  double y = filter.Calculate(kInput);
  EXPECT_NEAR(y, kInput, 1e-12);

  // And remain at steady state
  for (int i = 0; i < 20; ++i) {
    EXPECT_NEAR(filter.Calculate(kInput), kInput, 1e-12);
  }
}

TEST(BiquadFilterTest, DCGainConverges) {
  BiquadFilter filter({
      {0.00041659920440659937, 0.0008331984088131987, 0.00041659920440659937,
       -1.4796742169311934, 0.5558215432824889},
      {1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979},
  });

  constexpr double kInput = 2.5;
  double y = 0.0;
  for (int i = 0; i < 500; ++i) {
    y = filter.Calculate(kInput);
  }
  EXPECT_NEAR(y, kInput, 1e-6);  // Butterworth LP has DC gain 1
}

TEST(BiquadFilterTest, NumSections) {
  BiquadFilter one({{1.0, 0.0, 0.0, 0.0, 0.0}});
  EXPECT_EQ(one.NumSections(), 1u);

  BiquadFilter three({
      {1.0, 0.0, 0.0, 0.0, 0.0},
      {1.0, 0.0, 0.0, 0.0, 0.0},
      {1.0, 0.0, 0.0, 0.0, 0.0},
  });
  EXPECT_EQ(three.NumSections(), 3u);
}

TEST(BiquadFilterTest, EmptyCascadeThrows) {
  EXPECT_THROW(
      {
        std::vector<BiquadFilter::Section> sections;
        std::span<const BiquadFilter::Section> empty{sections};
        BiquadFilter filter{empty};
      },
      std::runtime_error);
}
