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

TEST_CASE("BiquadFilterEllipticTest LowPass4thOrderMatchesScipy",
          "[wpimath][filter]") {
  // scipy.signal.ellip(4, 1.0, 40.0, 50.0, btype='low', fs=1000.0)
  auto filter =
      BiquadFilter::Elliptic(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, 1.0, 40.0);
  auto sections = filter.Sections();
  REQUIRE(sections.size() == 2u);
  ExpectSectionNear(
      sections[0],
      {0.011738158079014929, -0.01231742214386518, 0.011738158079014929,
       -1.7624726990429698, 0.7947551993829407},
      1e-10);
  ExpectSectionNear(
      sections[1],
      {1.0, -1.7559103274197139, 1.0, -1.8423125689214854, 0.9369806105943849},
      1e-10);
}

TEST_CASE("BiquadFilterEllipticTest LowPassOddOrder5HasFirstOrderSection",
          "[wpimath][filter]") {
  // Odd order means one first-order-as-biquad section in the cascade (a2 = 0
  // and b2 = 0 for that section). scipy emits 3 sections — verify count and
  // shape rather than coefficient-by-coefficient, because section ordering
  // and zero pairing have the same scipy-vs-ours freedom as Butterworth BP.
  auto filter =
      BiquadFilter::Elliptic(Kind::LowPass, 5, 1000.0_Hz, 50.0_Hz, 1.0, 40.0);
  auto sections = filter.Sections();
  REQUIRE(sections.size() == 3u);

  int firstOrderSections = 0;
  for (const auto& s : sections) {
    if (s.a2 == 0.0 && s.b2 == 0.0) {
      ++firstOrderSections;
    }
  }
  CHECK(firstOrderSections == 1);
}

TEST_CASE("BiquadFilterEllipticTest LowPassEquirippleInPassbandAndStopband",
          "[wpimath][filter]") {
  // Even order: |H(0)| = -rippleDb at DC; odd order: |H(0)| = 0 dB.
  // Both share a stopband floor at -stopAttenDb.
  constexpr double kRipple = 1.0;
  constexpr double kAtten = 40.0;
  auto filter = BiquadFilter::Elliptic(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz,
                                       kRipple, kAtten);
  auto sections = filter.Sections();

  double dcDb = 20.0 * std::log10(CascadeMagnitude(sections, 0.0, 1000.0));
  double fcDb = 20.0 * std::log10(CascadeMagnitude(sections, 50.0, 1000.0));
  CHECK_NEAR(dcDb, -kRipple, 0.01);
  CHECK_NEAR(fcDb, -kRipple, 0.01);

  // Past the transition band the response stays at -rs floor.
  double stopDb = 20.0 * std::log10(CascadeMagnitude(sections, 100.0, 1000.0));
  CHECK(stopDb < -kAtten + 0.5);
}

TEST_CASE("BiquadFilterEllipticTest OddOrderHasUnityDcGain",
          "[wpimath][filter]") {
  auto filter =
      BiquadFilter::Elliptic(Kind::LowPass, 5, 1000.0_Hz, 50.0_Hz, 1.0, 40.0);
  double gainDc = CascadeMagnitude(filter.Sections(), 0.0, 1000.0);
  CHECK_NEAR(gainDc, 1.0, 1e-9);
}

TEST_CASE("BiquadFilterEllipticTest HighPassResponse", "[wpimath][filter]") {
  // Section ordering may differ from scipy for HP, like Cheby2 — verify the
  // total response instead.
  constexpr double kRipple = 1.0;
  constexpr double kAtten = 40.0;
  auto filter = BiquadFilter::Elliptic(Kind::HighPass, 4, 1000.0_Hz, 100.0_Hz,
                                       kRipple, kAtten);
  auto sections = filter.Sections();

  double passbandDb =
      20.0 * std::log10(CascadeMagnitude(sections, 400.0, 1000.0));
  CHECK_NEAR(passbandDb, 0.0, kRipple + 0.01);

  double cutoffDb =
      20.0 * std::log10(CascadeMagnitude(sections, 100.0, 1000.0));
  CHECK_NEAR(cutoffDb, -kRipple, 0.01);

  double dcDb = 20.0 * std::log10(CascadeMagnitude(sections, 0.0, 1000.0));
  CHECK(dcDb < -kAtten + 0.5);
}

TEST_CASE("BiquadFilterEllipticTest BandPass4thOrderMatchesScipy",
          "[wpimath][filter]") {
  // scipy.signal.ellip(4, 1.0, 40.0, [80.0, 120.0], btype='bandpass',
  // fs=1000.0)
  auto filter = BiquadFilter::Elliptic(Kind::BandPass, 4, 1000.0_Hz, 80.0_Hz,
                                       120.0_Hz, 1.0, 40.0);
  auto sections = filter.Sections();
  REQUIRE(sections.size() == 4u);
  ExpectSectionNear(
      sections[0],
      {0.010903156756394984, -0.008920205787636758, 0.010903156756394982,
       -1.4809043488404827, 0.9052184223450329},
      1e-10);
  ExpectSectionNear(sections[1],
                    {1.0, -1.9038045463534676, 0.9999999999999999,
                     -1.62699499510272, 0.9194678402475894},
                    1e-10);
  ExpectSectionNear(
      sections[2],
      {1.0, -1.3265553048553793, 1.0, -1.4370735618061194, 0.9697500844409095},
      1e-10);
  ExpectSectionNear(sections[3],
                    {1.0, -1.8057300347135379, 0.9999999999999998,
                     -1.733243724674222, 0.978571861817194},
                    1e-10);
}

TEST_CASE("BiquadFilterEllipticTest BandPassResponse", "[wpimath][filter]") {
  constexpr double kRipple = 1.0;
  constexpr double kAtten = 40.0;
  auto filter = BiquadFilter::Elliptic(Kind::BandPass, 3, 1000.0_Hz, 80.0_Hz,
                                       120.0_Hz, kRipple, kAtten);
  auto sections = filter.Sections();

  double centerDb =
      20.0 * std::log10(CascadeMagnitude(sections, 100.0, 1000.0));
  CHECK_NEAR(centerDb, 0.0, kRipple + 0.01);

  // Outside the band: deeply attenuated.
  CHECK(20.0 * std::log10(CascadeMagnitude(sections, 0.0, 1000.0)) <
        -kAtten + 1.0);
  CHECK(20.0 * std::log10(CascadeMagnitude(sections, 400.0, 1000.0)) <
        -kAtten + 1.0);
}

TEST_CASE("BiquadFilterEllipticTest RejectsInvalidArgs", "[wpimath][filter]") {
  // Order, fs, ripple, atten ranges.
  CHECK_THROWS_AS(
      BiquadFilter::Elliptic(Kind::LowPass, 0, 1000.0_Hz, 50.0_Hz, 1.0, 40.0),
      std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::Elliptic(Kind::LowPass, 4, 0.0_Hz, 50.0_Hz, 1.0, 40.0),
      std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::Elliptic(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, 0.0, 40.0),
      std::invalid_argument);
  // Stopband must be deeper than passband ripple.
  CHECK_THROWS_AS(
      BiquadFilter::Elliptic(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, 40.0, 1.0),
      std::invalid_argument);
  CHECK_THROWS_AS(
      BiquadFilter::Elliptic(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz, 5.0, 5.0),
      std::invalid_argument);
  // Frequencies out of range / inverted.
  CHECK_THROWS_AS(
      BiquadFilter::Elliptic(Kind::LowPass, 4, 1000.0_Hz, 600.0_Hz, 1.0, 40.0),
      std::invalid_argument);
  CHECK_THROWS_AS(BiquadFilter::Elliptic(Kind::BandPass, 4, 1000.0_Hz, 120.0_Hz,
                                         80.0_Hz, 1.0, 40.0),
                  std::invalid_argument);
}

}  // namespace
