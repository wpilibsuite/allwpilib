// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <complex>
#include <numbers>
#include <span>
#include <stdexcept>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/filter/BiquadFilter.hpp"
#include "wpi/units/frequency.hpp"

namespace {

using wpi::math::BiquadFilter;
using Section = BiquadFilter::Section;
using Kind = BiquadFilter::Kind;

double CascadeMagnitude(std::span<const Section> sos, double f, double fs) {
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
  CHECK_NEAR(got.b0, want.b0, tol);
  CHECK_NEAR(got.b1, want.b1, tol);
  CHECK_NEAR(got.b2, want.b2, tol);
  CHECK_NEAR(got.a1, want.a1, tol);
  CHECK_NEAR(got.a2, want.a2, tol);
}

// ----- Chebyshev type I ----------------------------------------------------

TEST_CASE("BiquadFilterChebyshevTest Cheby1LowPass4thOrderMatchesScipy",
          "[wpimath][filter]") {
  // scipy.signal.cheby1(4, 1.0, 50.0, btype='low', fs=1000.0, output='sos')
  auto filter =
      BiquadFilter::ChebyshevI(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, 1.0);
  auto sections = filter.Sections();
  REQUIRE(sections.size() == 2u);
  ExpectSectionNear(
      sections[0],
      {0.00012984963538691335, 0.0002596992707738267, 0.00012984963538691335,
       -1.7831991339963722, 0.8083720161261031},
      1e-10);
  ExpectSectionNear(sections[1],
                    {1.0, 2.0, 1.0, -1.8246970351326663, 0.917300614770565},
                    1e-10);
}

TEST_CASE("BiquadFilterChebyshevTest Cheby1HighPass4thOrderMatchesScipy",
          "[wpimath][filter]") {
  // scipy.signal.cheby1(4, 1.0, 100.0, btype='high', fs=1000.0, output='sos')
  auto filter =
      BiquadFilter::ChebyshevI(Kind::HighPass, 4, 1000.0_Hz, 100.0_Hz, 1.0);
  auto sections = filter.Sections();
  REQUIRE(sections.size() == 2u);
  ExpectSectionNear(
      sections[0],
      {0.3439348735216468, -0.6878697470432936, 0.3439348735216468,
       -0.5756927885601547, 0.2749869650540311},
      1e-10);
  ExpectSectionNear(sections[1],
                    {1.0, -2.0, 1.0, -1.4896289697923346, 0.8466697013585162},
                    1e-10);
}

TEST_CASE("BiquadFilterChebyshevTest Cheby1BandPass4thOrderMatchesScipy",
          "[wpimath][filter]") {
  // scipy.signal.cheby1(4, 1.0, [80.0, 120.0], btype='bandpass', fs=1000.0)
  auto filter = BiquadFilter::ChebyshevI(Kind::BandPass, 4, 1000.0_Hz, 80.0_Hz,
                                         120.0_Hz, 1.0);
  auto sections = filter.Sections();
  REQUIRE(sections.size() == 4u);
  ExpectSectionNear(
      sections[0],
      {5.463638752463053e-05, 0.00010927277504926106, 5.463638752463053e-05,
       -1.4985467271298947, 0.9129301418072939},
      1e-10);
  ExpectSectionNear(sections[1],
                    {1.0, 2.0, 1.0, -1.6224939133759921, 0.9242414431352561},
                    1e-10);
  ExpectSectionNear(sections[2],
                    {1.0, -2.0, 1.0, -1.4320495577056345, 0.9601480937923097},
                    1e-10);
  ExpectSectionNear(sections[3],
                    {1.0, -2.0, 1.0, -1.7261705273848356, 0.9716328706093393},
                    1e-10);
}

TEST_CASE("BiquadFilterChebyshevTest Cheby1LowPassPassbandStaysWithinRipple",
          "[wpimath][filter]") {
  constexpr double kRippleDb = 1.0;
  auto filter =
      BiquadFilter::ChebyshevI(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, kRippleDb);
  auto sections = filter.Sections();

  // For even order, |H(0)| = 1/sqrt(1+eps^2) — i.e. -ripple dB at DC.
  double gainDc = CascadeMagnitude(sections, 0.0, 1000.0);
  double dcDb = 20.0 * std::log10(gainDc);
  CHECK_NEAR(dcDb, -kRippleDb, 0.01);

  // |H(fc)| = 1/sqrt(1+eps^2) too (ripple boundary).
  double gainFc = CascadeMagnitude(sections, 50.0, 1000.0);
  double fcDb = 20.0 * std::log10(gainFc);
  CHECK_NEAR(fcDb, -kRippleDb, 0.01);

  // Strong attenuation past the cutoff.
  double gainStop = CascadeMagnitude(sections, 200.0, 1000.0);
  CHECK(20.0 * std::log10(gainStop) < -40.0);
}

TEST_CASE("BiquadFilterChebyshevTest Cheby1OddOrderHasUnityDcGain",
          "[wpimath][filter]") {
  // For odd order the ripple boundary touches |H(0)| = 1 exactly.
  auto filter =
      BiquadFilter::ChebyshevI(Kind::LowPass, 5, 1000.0_Hz, 50.0_Hz, 1.0);
  double gainDc = CascadeMagnitude(filter.Sections(), 0.0, 1000.0);
  CHECK_NEAR(gainDc, 1.0, 1e-9);
}

TEST_CASE("BiquadFilterChebyshevTest Cheby1RejectsInvalidArgs",
          "[wpimath][filter]") {
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevI(Kind::LowPass, 0, 1000.0_Hz, 50.0_Hz, 1.0),
      std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevI(Kind::LowPass, 4, 0.0_Hz, 50.0_Hz, 1.0),
      std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevI(Kind::LowPass, 4, 1000.0_Hz, 0.0_Hz, 1.0),
      std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevI(Kind::LowPass, 4, 1000.0_Hz, 600.0_Hz, 1.0),
      std::invalid_argument);
  CHECK_THROWS_AS(BiquadFilter::ChebyshevI(Kind::BandPass, 4, 1000.0_Hz,
                                           120.0_Hz, 80.0_Hz, 1.0),
                  std::invalid_argument);
  // Ripple must be strictly positive.
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevI(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, 0.0),
      std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevI(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, -1.0),
      std::invalid_argument);
}

// ----- Chebyshev type II ---------------------------------------------------

TEST_CASE("BiquadFilterChebyshevTest Cheby2LowPass4thOrderMatchesScipy",
          "[wpimath][filter]") {
  // scipy.signal.cheby2(4, 40.0, 50.0, btype='low', fs=1000.0, output='sos')
  auto filter =
      BiquadFilter::ChebyshevII(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, 40.0);
  auto sections = filter.Sections();
  REQUIRE(sections.size() == 2u);
  ExpectSectionNear(
      sections[0],
      {0.009735570656077937, -0.01377605024474192, 0.009735570656077937,
       -1.6993957730842835, 0.7262535657383176},
      1e-10);
  ExpectSectionNear(
      sections[1],
      {1.0, -1.8857977835164716, 1.0, -1.87354703561714, 0.8977631739778823},
      1e-10);
}

TEST_CASE("BiquadFilterChebyshevTest Cheby2HighPassResponse",
          "[wpimath][filter]") {
  // For HP/BS, zero pairings can differ from scipy without affecting the
  // cascade response (same caveat as Butterworth BP/BS). Verify the response
  // at points that uniquely characterize the filter rather than per-section
  // coefficients.
  constexpr double kAttenDb = 40.0;
  auto filter = BiquadFilter::ChebyshevII(Kind::HighPass, 4, 1000.0_Hz,
                                          100.0_Hz, kAttenDb);
  auto sections = filter.Sections();

  // Passband (high frequencies): unity gain.
  double gainHigh = CascadeMagnitude(sections, 400.0, 1000.0);
  CHECK_NEAR(gainHigh, 1.0, 1e-3);

  // Stopband edge fc=100: response reaches the stopband attenuation.
  double gainFc = CascadeMagnitude(sections, 100.0, 1000.0);
  CHECK(20.0 * std::log10(gainFc) < -kAttenDb + 0.01);

  // DC: deeply attenuated.
  double gainDc = CascadeMagnitude(sections, 0.0, 1000.0);
  CHECK(20.0 * std::log10(gainDc) < -kAttenDb + 0.5);
}

TEST_CASE("BiquadFilterChebyshevTest Cheby2BandStopResponse",
          "[wpimath][filter]") {
  // BandStop: zero pairings differ from scipy in the same way as Butterworth
  // BS — total response matches but per-section coefficients don't.
  constexpr double kAttenDb = 40.0;
  auto filter = BiquadFilter::ChebyshevII(Kind::BandStop, 4, 1000.0_Hz, 80.0_Hz,
                                          120.0_Hz, kAttenDb);
  auto sections = filter.Sections();

  // Outside the stop band: unity gain.
  CHECK_NEAR(CascadeMagnitude(sections, 0.0, 1000.0), 1.0, 1e-3);
  CHECK_NEAR(CascadeMagnitude(sections, 400.0, 1000.0), 1.0, 1e-3);

  // Stop-band edges hit the attenuation level; deeper inside the band is
  // at least that attenuated.
  CHECK(20.0 * std::log10(CascadeMagnitude(sections, 80.0, 1000.0)) <
        -kAttenDb + 0.01);
  CHECK(20.0 * std::log10(CascadeMagnitude(sections, 120.0, 1000.0)) <
        -kAttenDb + 0.01);
}

TEST_CASE(
    "BiquadFilterChebyshevTest Cheby2LowPassFlatPassbandRipplesInStopband",
    "[wpimath][filter]") {
  constexpr double kAttenDb = 40.0;
  auto filter =
      BiquadFilter::ChebyshevII(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, kAttenDb);
  auto sections = filter.Sections();

  // Cheby2 has |H(0)| = 1 always (no DC ripple).
  double gainDc = CascadeMagnitude(sections, 0.0, 1000.0);
  CHECK_NEAR(gainDc, 1.0, 1e-6);

  // At the stopband edge fc=50, |H| reaches the stopband attenuation.
  double gainFc = CascadeMagnitude(sections, 50.0, 1000.0);
  CHECK(20.0 * std::log10(gainFc) < -kAttenDb + 0.01);

  // Stopband stays at or below kAttenDb attenuation.
  double gainDeep = CascadeMagnitude(sections, 100.0, 1000.0);
  CHECK(20.0 * std::log10(gainDeep) < -kAttenDb + 0.5);
}

TEST_CASE("BiquadFilterChebyshevTest Cheby2RejectsInvalidArgs",
          "[wpimath][filter]") {
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevII(Kind::LowPass, 0, 1000.0_Hz, 50.0_Hz, 40.0),
      std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevII(Kind::LowPass, 4, 0.0_Hz, 50.0_Hz, 40.0),
      std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevII(Kind::LowPass, 4, 1000.0_Hz, 0.0_Hz, 40.0),
      std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevII(Kind::LowPass, 4, 1000.0_Hz, 600.0_Hz, 40.0),
      std::invalid_argument);
  CHECK_THROWS_AS(BiquadFilter::ChebyshevII(Kind::BandPass, 4, 1000.0_Hz,
                                            120.0_Hz, 80.0_Hz, 40.0),
                  std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevII(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, 0.0),
      std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::ChebyshevII(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, -10.0),
      std::invalid_argument);
}

}  // namespace
