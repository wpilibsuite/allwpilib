// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/design/FilterDesign.hpp"

#include <array>
#include <cmath>
#include <complex>
#include <numbers>

#include <gtest/gtest.h>
#include <wpi/math/filter/BiquadFilter.hpp>

namespace {

using wpi::filterdesigner::DesignButterworth;
using wpi::filterdesigner::DesignMovingAverage;
using wpi::filterdesigner::DesignNotch;
using wpi::filterdesigner::FilterKind;
using wpi::filterdesigner::Section;
using wpi::filterdesigner::Sections;

// |H(e^{j·2π·f/fs})| across a cascade of biquad sections.
double CascadeMagnitude(const Sections& sos, double f, double fs) {
  const double w = 2.0 * std::numbers::pi * f / fs;
  const std::complex<double> z1 = std::polar(1.0, -w);
  const std::complex<double> z2 = std::polar(1.0, -2.0 * w);
  std::complex<double> h{1.0, 0.0};
  for (const auto& s : sos) {
    std::complex<double> num = s.b0 + s.b1 * z1 + s.b2 * z2;
    std::complex<double> den = 1.0 + s.a1 * z1 + s.a2 * z2;
    h *= num / den;
  }
  return std::abs(h);
}

void ExpectSectionNear(const Section& got, const Section& want, double tol) {
  EXPECT_NEAR(got.b0, want.b0, tol);
  EXPECT_NEAR(got.b1, want.b1, tol);
  EXPECT_NEAR(got.b2, want.b2, tol);
  EXPECT_NEAR(got.a1, want.a1, tol);
  EXPECT_NEAR(got.a2, want.a2, tol);
}

TEST(FilterDesignTest, ButterworthLowPass4thOrderMatchesScipy) {
  // scipy.signal.butter(4, 50.0, btype='low', fs=1000.0, output='sos')
  auto result = DesignButterworth(FilterKind::LowPass, 4, 1000.0, 50.0);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 2u);
  ExpectSectionNear(
      (*result)[0],
      {0.00041659920440659937, 0.0008331984088131987, 0.00041659920440659937,
       -1.4796742169311934, 0.5558215432824889},
      1e-10);
  ExpectSectionNear((*result)[1],
                    {1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979},
                    1e-10);
}

TEST(FilterDesignTest, ButterworthLowPass8thOrderMatchesScipy) {
  // scipy.signal.butter(8, 100.0, btype='low', fs=1000.0, output='sos')
  auto result = DesignButterworth(FilterKind::LowPass, 8, 1000.0, 100.0);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 4u);
  ExpectSectionNear(
      (*result)[0],
      {2.3959644103776166e-05, 4.791928820755233e-05, 2.3959644103776166e-05,
       -1.0263514742610553, 0.26864019099379005},
      1e-10);
  ExpectSectionNear((*result)[1],
                    {1.0, 2.0, 1.0, -1.0868584613628944, 0.343430940165366},
                    1e-10);
  ExpectSectionNear((*result)[2],
                    {1.0, 2.0, 1.0, -1.2197253651240232, 0.5076634651740437},
                    1e-10);
  ExpectSectionNear((*result)[3],
                    {1.0, 2.0, 1.0, -1.4515795942478362, 0.794251053241888},
                    1e-10);
}

TEST(FilterDesignTest, ButterworthLowPassCutoffIsMinusThreeDb) {
  auto result = DesignButterworth(FilterKind::LowPass, 4, 1000.0, 50.0);
  ASSERT_TRUE(result.has_value());
  double gainDc = CascadeMagnitude(*result, 0.0, 1000.0);
  double gainFc = CascadeMagnitude(*result, 50.0, 1000.0);
  EXPECT_NEAR(gainDc, 1.0, 1e-10);
  EXPECT_NEAR(20.0 * std::log10(gainFc), -3.0, 0.05);
}

TEST(FilterDesignTest, ButterworthHighPassResponse) {
  auto result = DesignButterworth(FilterKind::HighPass, 4, 1000.0, 100.0);
  ASSERT_TRUE(result.has_value());
  double gainDc = CascadeMagnitude(*result, 0.0, 1000.0);
  double gainFc = CascadeMagnitude(*result, 100.0, 1000.0);
  double gainHigh = CascadeMagnitude(*result, 400.0, 1000.0);
  EXPECT_NEAR(gainDc, 0.0, 1e-10);
  EXPECT_NEAR(20.0 * std::log10(gainFc), -3.0, 0.05);
  EXPECT_NEAR(gainHigh, 1.0, 1e-3);
}

TEST(FilterDesignTest, ButterworthBandPassResponse) {
  auto result = DesignButterworth(FilterKind::BandPass, 4, 1000.0, 80.0, 120.0);
  ASSERT_TRUE(result.has_value());
  double gainDc = CascadeMagnitude(*result, 0.0, 1000.0);
  double gainCenter = CascadeMagnitude(*result, 100.0, 1000.0);
  double gainNyquist = CascadeMagnitude(*result, 499.0, 1000.0);
  EXPECT_LT(gainDc, 1e-6);
  EXPECT_LT(gainNyquist, 1e-6);
  EXPECT_GT(gainCenter, 0.8);
}

TEST(FilterDesignTest, ButterworthBandStopResponse) {
  auto result = DesignButterworth(FilterKind::BandStop, 4, 1000.0, 80.0, 120.0);
  ASSERT_TRUE(result.has_value());
  double gainDc = CascadeMagnitude(*result, 0.0, 1000.0);
  // Geometric mean is the analog-prototype zero; digital zero is nearby.
  double gainCenter =
      CascadeMagnitude(*result, std::sqrt(80.0 * 120.0), 1000.0);
  double gainNyquist = CascadeMagnitude(*result, 499.0, 1000.0);
  EXPECT_NEAR(gainDc, 1.0, 1e-3);
  EXPECT_NEAR(gainNyquist, 1.0, 1e-3);
  EXPECT_LT(gainCenter, 1e-6);
}

TEST(FilterDesignTest, ButterworthRejectsInvalidArgs) {
  EXPECT_FALSE(DesignButterworth(FilterKind::LowPass, 0, 1000.0, 50.0));
  EXPECT_FALSE(DesignButterworth(FilterKind::LowPass, 4, 0.0, 50.0));
  EXPECT_FALSE(DesignButterworth(FilterKind::LowPass, 4, 1000.0, 0.0));
  EXPECT_FALSE(DesignButterworth(FilterKind::LowPass, 4, 1000.0, 500.0));
  EXPECT_FALSE(DesignButterworth(FilterKind::LowPass, 4, 1000.0, 600.0));
  EXPECT_FALSE(DesignButterworth(FilterKind::BandPass, 4, 1000.0, 120.0, 80.0));
  EXPECT_FALSE(DesignButterworth(FilterKind::BandPass, 4, 1000.0, 80.0, 80.0));
  EXPECT_FALSE(DesignButterworth(FilterKind::BandStop, 4, 1000.0, 80.0, 500.0));
}

TEST(FilterDesignTest, Notch60HzMatchesScipy) {
  // scipy.signal.iirnotch(60.0, Q=10.0, fs=1000.0)
  auto result = DesignNotch(1000.0, 60.0, 10.0);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 1u);
  ExpectSectionNear(
      (*result)[0],
      {0.9814970254751076, -1.8251457105120343, 0.9814970254751076,
       -1.8251457105120341, 0.9629940509502151},
      1e-12);
}

TEST(FilterDesignTest, NotchAttenuatesAtCenter) {
  auto result = DesignNotch(1000.0, 60.0, 10.0);
  ASSERT_TRUE(result.has_value());
  double gainDc = CascadeMagnitude(*result, 0.0, 1000.0);
  double gainNotch = CascadeMagnitude(*result, 60.0, 1000.0);
  double gainFar = CascadeMagnitude(*result, 200.0, 1000.0);
  EXPECT_NEAR(gainDc, 1.0, 1e-6);
  EXPECT_LT(gainNotch, 1e-6);
  EXPECT_NEAR(gainFar, 1.0, 0.05);
}

TEST(FilterDesignTest, NotchRejectsInvalidArgs) {
  EXPECT_FALSE(DesignNotch(0.0, 60.0, 10.0));
  EXPECT_FALSE(DesignNotch(-1000.0, 60.0, 10.0));
  EXPECT_FALSE(DesignNotch(1000.0, 0.0, 10.0));
  EXPECT_FALSE(DesignNotch(1000.0, 500.0, 10.0));
  EXPECT_FALSE(DesignNotch(1000.0, 600.0, 10.0));
  EXPECT_FALSE(DesignNotch(1000.0, 60.0, 0.0));
  EXPECT_FALSE(DesignNotch(1000.0, 60.0, -1.0));
}

TEST(FilterDesignTest, MovingAverageSingleTapIsPassThrough) {
  auto result = DesignMovingAverage(1);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 1u);
  ExpectSectionNear((*result)[0], {1.0, 0.0, 0.0, 0.0, 0.0}, 0.0);
}

TEST(FilterDesignTest, MovingAverageEvenLengthHasUnitDcGainAndNyquistNull) {
  auto result = DesignMovingAverage(4);
  ASSERT_TRUE(result.has_value());
  double gainDc = CascadeMagnitude(*result, 0.0, 1000.0);
  double gainNyquist = CascadeMagnitude(*result, 500.0, 1000.0);
  EXPECT_NEAR(gainDc, 1.0, 1e-12);
  EXPECT_LT(gainNyquist, 1e-12);
}

TEST(FilterDesignTest, MovingAverageOddLengthNullsAtFsOverN) {
  constexpr double kFs = 1000.0;
  constexpr int kN = 5;
  auto result = DesignMovingAverage(kN);
  ASSERT_TRUE(result.has_value());
  double gainDc = CascadeMagnitude(*result, 0.0, kFs);
  double gainNull = CascadeMagnitude(*result, kFs / kN, kFs);
  double gainHalfNull = CascadeMagnitude(*result, kFs / (2.0 * kN), kFs);
  EXPECT_NEAR(gainDc, 1.0, 1e-12);
  EXPECT_LT(gainNull, 1e-10);
  EXPECT_GT(gainHalfNull, 0.1);
}

TEST(FilterDesignTest, MovingAverageMatchesSumAverageImpulseResponse) {
  // Verify impulse response of the cascade equals 1/N for N samples, then 0.
  constexpr int kN = 7;
  auto result = DesignMovingAverage(kN);
  ASSERT_TRUE(result.has_value());

  wpi::math::BiquadFilter filter(*result);
  std::array<double, kN + 3> out{};
  for (size_t i = 0; i < out.size(); ++i) {
    double x = (i == 0) ? 1.0 : 0.0;
    out[i] = filter.Calculate(x);
  }
  for (int i = 0; i < kN; ++i) {
    EXPECT_NEAR(out[i], 1.0 / kN, 1e-12) << "tap " << i;
  }
  for (size_t i = kN; i < out.size(); ++i) {
    EXPECT_NEAR(out[i], 0.0, 1e-12) << "post-window " << i;
  }
}

TEST(FilterDesignTest, MovingAverageRejectsInvalidArgs) {
  EXPECT_FALSE(DesignMovingAverage(0));
  EXPECT_FALSE(DesignMovingAverage(-1));
}

}  // namespace
