// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <complex>
#include <numbers>

#include <gtest/gtest.h>

#include "wpi/filterdesigner/design/FilterDesign.hpp"

namespace {

using wpi::filterdesigner::DesignChebyshev1;
using wpi::filterdesigner::DesignChebyshev2;
using wpi::filterdesigner::FilterKind;
using wpi::filterdesigner::Section;
using wpi::filterdesigner::Sections;

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

// ----- Chebyshev type I
// -------------------------------------------------------

TEST(ChebyshevTest, Cheby1LowPass4thOrderMatchesScipy) {
  // scipy.signal.cheby1(4, 1.0, 50.0, btype='low', fs=1000.0, output='sos')
  auto result =
      DesignChebyshev1(FilterKind::LowPass, 4, 1000.0, 50.0, 0.0, 1.0);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 2u);
  ExpectSectionNear(
      (*result)[0],
      {0.00012984963538691335, 0.0002596992707738267, 0.00012984963538691335,
       -1.7831991339963722, 0.8083720161261031},
      1e-10);
  ExpectSectionNear((*result)[1],
                    {1.0, 2.0, 1.0, -1.8246970351326663, 0.917300614770565},
                    1e-10);
}

TEST(ChebyshevTest, Cheby1HighPass4thOrderMatchesScipy) {
  // scipy.signal.cheby1(4, 1.0, 100.0, btype='high', fs=1000.0, output='sos')
  auto result =
      DesignChebyshev1(FilterKind::HighPass, 4, 1000.0, 100.0, 0.0, 1.0);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 2u);
  ExpectSectionNear(
      (*result)[0],
      {0.3439348735216468, -0.6878697470432936, 0.3439348735216468,
       -0.5756927885601547, 0.2749869650540311},
      1e-10);
  ExpectSectionNear((*result)[1],
                    {1.0, -2.0, 1.0, -1.4896289697923346, 0.8466697013585162},
                    1e-10);
}

TEST(ChebyshevTest, Cheby1BandPass4thOrderMatchesScipy) {
  // scipy.signal.cheby1(4, 1.0, [80.0, 120.0], btype='bandpass', fs=1000.0)
  auto result =
      DesignChebyshev1(FilterKind::BandPass, 4, 1000.0, 80.0, 120.0, 1.0);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 4u);
  ExpectSectionNear(
      (*result)[0],
      {5.463638752463053e-05, 0.00010927277504926106, 5.463638752463053e-05,
       -1.4985467271298947, 0.9129301418072939},
      1e-10);
  ExpectSectionNear((*result)[1],
                    {1.0, 2.0, 1.0, -1.6224939133759921, 0.9242414431352561},
                    1e-10);
  ExpectSectionNear((*result)[2],
                    {1.0, -2.0, 1.0, -1.4320495577056345, 0.9601480937923097},
                    1e-10);
  ExpectSectionNear((*result)[3],
                    {1.0, -2.0, 1.0, -1.7261705273848356, 0.9716328706093393},
                    1e-10);
}

TEST(ChebyshevTest, Cheby1LowPassPassbandStaysWithinRipple) {
  constexpr double kRippleDb = 1.0;
  auto result =
      DesignChebyshev1(FilterKind::LowPass, 4, 1000.0, 50.0, 0.0, kRippleDb);
  ASSERT_TRUE(result.has_value());

  // For even order, |H(0)| = 1/sqrt(1+eps^2) — i.e. -ripple dB at DC.
  double gainDc = CascadeMagnitude(*result, 0.0, 1000.0);
  double dcDb = 20.0 * std::log10(gainDc);
  EXPECT_NEAR(dcDb, -kRippleDb, 0.01);

  // |H(fc)| = 1/sqrt(1+eps^2) too (ripple boundary).
  double gainFc = CascadeMagnitude(*result, 50.0, 1000.0);
  double fcDb = 20.0 * std::log10(gainFc);
  EXPECT_NEAR(fcDb, -kRippleDb, 0.01);

  // Strong attenuation past the cutoff.
  double gainStop = CascadeMagnitude(*result, 200.0, 1000.0);
  EXPECT_LT(20.0 * std::log10(gainStop), -40.0);
}

TEST(ChebyshevTest, Cheby1OddOrderHasUnityDcGain) {
  // For odd order the ripple boundary touches |H(0)| = 1 exactly.
  auto result =
      DesignChebyshev1(FilterKind::LowPass, 5, 1000.0, 50.0, 0.0, 1.0);
  ASSERT_TRUE(result.has_value());
  double gainDc = CascadeMagnitude(*result, 0.0, 1000.0);
  EXPECT_NEAR(gainDc, 1.0, 1e-9);
}

TEST(ChebyshevTest, Cheby1RejectsInvalidArgs) {
  EXPECT_FALSE(
      DesignChebyshev1(FilterKind::LowPass, 0, 1000.0, 50.0, 0.0, 1.0));
  EXPECT_FALSE(DesignChebyshev1(FilterKind::LowPass, 4, 0.0, 50.0, 0.0, 1.0));
  EXPECT_FALSE(DesignChebyshev1(FilterKind::LowPass, 4, 1000.0, 0.0, 0.0, 1.0));
  EXPECT_FALSE(
      DesignChebyshev1(FilterKind::LowPass, 4, 1000.0, 600.0, 0.0, 1.0));
  EXPECT_FALSE(
      DesignChebyshev1(FilterKind::BandPass, 4, 1000.0, 120.0, 80.0, 1.0));
  // Ripple must be strictly positive.
  EXPECT_FALSE(
      DesignChebyshev1(FilterKind::LowPass, 4, 1000.0, 50.0, 0.0, 0.0));
  EXPECT_FALSE(
      DesignChebyshev1(FilterKind::LowPass, 4, 1000.0, 50.0, 0.0, -1.0));
}

// ----- Chebyshev type II
// ------------------------------------------------------

TEST(ChebyshevTest, Cheby2LowPass4thOrderMatchesScipy) {
  // scipy.signal.cheby2(4, 40.0, 50.0, btype='low', fs=1000.0, output='sos')
  auto result =
      DesignChebyshev2(FilterKind::LowPass, 4, 1000.0, 50.0, 0.0, 40.0);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 2u);
  ExpectSectionNear(
      (*result)[0],
      {0.009735570656077937, -0.01377605024474192, 0.009735570656077937,
       -1.6993957730842835, 0.7262535657383176},
      1e-10);
  ExpectSectionNear(
      (*result)[1],
      {1.0, -1.8857977835164716, 1.0, -1.87354703561714, 0.8977631739778823},
      1e-10);
}

TEST(ChebyshevTest, Cheby2HighPassResponse) {
  // For HP/BS, zero pairings can differ from scipy without affecting the
  // cascade response (same caveat as Butterworth BP/BS). Verify the response
  // at points that uniquely characterize the filter rather than per-section
  // coefficients.
  constexpr double kAttenDb = 40.0;
  auto result =
      DesignChebyshev2(FilterKind::HighPass, 4, 1000.0, 100.0, 0.0, kAttenDb);
  ASSERT_TRUE(result.has_value());

  // Passband (high frequencies): unity gain.
  double gainHigh = CascadeMagnitude(*result, 400.0, 1000.0);
  EXPECT_NEAR(gainHigh, 1.0, 1e-3);

  // Stopband edge fc=100: response reaches the stopband attenuation.
  double gainFc = CascadeMagnitude(*result, 100.0, 1000.0);
  EXPECT_LT(20.0 * std::log10(gainFc), -kAttenDb + 0.01);

  // DC: deeply attenuated.
  double gainDc = CascadeMagnitude(*result, 0.0, 1000.0);
  EXPECT_LT(20.0 * std::log10(gainDc), -kAttenDb + 0.5);
}

TEST(ChebyshevTest, Cheby2BandStopResponse) {
  // BandStop: zero pairings differ from scipy in the same way as Butterworth
  // BS — total response matches but per-section coefficients don't.
  constexpr double kAttenDb = 40.0;
  auto result =
      DesignChebyshev2(FilterKind::BandStop, 4, 1000.0, 80.0, 120.0, kAttenDb);
  ASSERT_TRUE(result.has_value());

  // Outside the stop band: unity gain.
  EXPECT_NEAR(CascadeMagnitude(*result, 0.0, 1000.0), 1.0, 1e-3);
  EXPECT_NEAR(CascadeMagnitude(*result, 400.0, 1000.0), 1.0, 1e-3);

  // Stop-band edges hit the attenuation level; deeper inside the band is
  // at least that attenuated.
  EXPECT_LT(20.0 * std::log10(CascadeMagnitude(*result, 80.0, 1000.0)),
            -kAttenDb + 0.01);
  EXPECT_LT(20.0 * std::log10(CascadeMagnitude(*result, 120.0, 1000.0)),
            -kAttenDb + 0.01);
}

TEST(ChebyshevTest, Cheby2LowPassFlatPassbandRipplesInStopband) {
  constexpr double kAttenDb = 40.0;
  auto result =
      DesignChebyshev2(FilterKind::LowPass, 4, 1000.0, 50.0, 0.0, kAttenDb);
  ASSERT_TRUE(result.has_value());

  // Cheby2 has |H(0)| = 1 always (no DC ripple).
  double gainDc = CascadeMagnitude(*result, 0.0, 1000.0);
  EXPECT_NEAR(gainDc, 1.0, 1e-6);

  // At the stopband edge fc=50, |H| reaches the stopband attenuation.
  double gainFc = CascadeMagnitude(*result, 50.0, 1000.0);
  EXPECT_LT(20.0 * std::log10(gainFc), -kAttenDb + 0.01);

  // Stopband stays at or below kAttenDb attenuation.
  double gainDeep = CascadeMagnitude(*result, 100.0, 1000.0);
  EXPECT_LT(20.0 * std::log10(gainDeep), -kAttenDb + 0.5);
}

TEST(ChebyshevTest, Cheby2RejectsInvalidArgs) {
  EXPECT_FALSE(
      DesignChebyshev2(FilterKind::LowPass, 0, 1000.0, 50.0, 0.0, 40.0));
  EXPECT_FALSE(DesignChebyshev2(FilterKind::LowPass, 4, 0.0, 50.0, 0.0, 40.0));
  EXPECT_FALSE(
      DesignChebyshev2(FilterKind::LowPass, 4, 1000.0, 0.0, 0.0, 40.0));
  EXPECT_FALSE(
      DesignChebyshev2(FilterKind::LowPass, 4, 1000.0, 600.0, 0.0, 40.0));
  EXPECT_FALSE(
      DesignChebyshev2(FilterKind::BandPass, 4, 1000.0, 120.0, 80.0, 40.0));
  EXPECT_FALSE(
      DesignChebyshev2(FilterKind::LowPass, 4, 1000.0, 50.0, 0.0, 0.0));
  EXPECT_FALSE(
      DesignChebyshev2(FilterKind::LowPass, 4, 1000.0, 50.0, 0.0, -10.0));
}

}  // namespace
