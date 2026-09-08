// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/PoleZero.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <numbers>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
#include "wpi/filterdesigner/model/Stage.hpp"
#include "wpi/math/filter/BiquadFilter.hpp"
#include "wpi/units/frequency.hpp"

namespace {

using wpi::filterdesigner::ComputePolesZeros;
using wpi::filterdesigner::Section;
using wpi::filterdesigner::Sections;
using wpi::math::BiquadFilter;
using namespace wpi::units;

Sections SectionsOf(const BiquadFilter& f) {
  auto span = f.Sections();
  return Sections(span.begin(), span.end());
}

TEST_CASE("PoleZeroTest EmptyCascadeHasNoRoots", "[filterdesigner]") {
  Sections empty;
  auto pz = ComputePolesZeros(empty);
  CHECK(pz.poles.empty());
  CHECK(pz.zeros.empty());
}

TEST_CASE("PoleZeroTest PassThroughHasPolesAndZerosAtOrigin",
          "[filterdesigner]") {
  // b0=1, everything else 0 — H(z) = 1. The quadratic-as-stored form is
  // 1*z^2 + 0*z + 0, root at 0 (double), same on the denominator (a1=a2=0).
  Sections s{Section{1.0, 0.0, 0.0, 0.0, 0.0}};
  auto pz = ComputePolesZeros(s);
  REQUIRE(pz.poles.size() == 2u);
  REQUIRE(pz.zeros.size() == 2u);
  for (const auto& p : pz.poles) {
    CHECK_NEAR(std::abs(p), 0.0, 1e-12);
  }
  for (const auto& z : pz.zeros) {
    CHECK_NEAR(std::abs(z), 0.0, 1e-12);
  }
}

TEST_CASE("PoleZeroTest ButterworthLowPassPolesInsideUnitCircle",
          "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  auto pz = ComputePolesZeros(filter);
  // 2 biquad sections × 2 poles = 4 poles.
  CHECK(pz.poles.size() == 4u);
  // Stability: every pole strictly inside the unit circle.
  for (const auto& p : pz.poles) {
    UNSCOPED_INFO("pole at (" << p.real() << ", " << p.imag()
                              << ") not inside unit circle");
    CHECK(std::abs(p) < 1.0 - 1e-6);
  }
}

TEST_CASE("PoleZeroTest ButterworthLowPassZerosAtNyquist", "[filterdesigner]") {
  // The bilinear transform maps analog zeros at infinity to z = -1, so a
  // 4th-order Butterworth LP has four zeros there. The 1e-6 tolerance is for
  // the repeated real roots, which the quadratic formula returns with a
  // spurious imaginary part around sqrt(machine epsilon).
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  auto pz = ComputePolesZeros(filter);
  REQUIRE(pz.zeros.size() == 4u);
  for (const auto& z : pz.zeros) {
    UNSCOPED_INFO("zero at (" << z.real() << ", " << z.imag()
                              << ") not near Nyquist");
    CHECK(std::abs(z - std::complex<double>{-1.0, 0.0}) < 1e-6);
  }
}

TEST_CASE("PoleZeroTest ButterworthHighPassZerosAtDC", "[filterdesigner]") {
  // High-pass: analog zeros at 0 map to z = 1. At order 3 the cascade is one
  // biquad, numerator (z-1)^2, plus a first-order section whose biquad form is
  // z(z-1) — a third zero at unity and a structural one at the origin.
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::HighPass, 3, 1000_Hz, 100_Hz));
  auto pz = ComputePolesZeros(filter);
  REQUIRE(pz.zeros.size() == 4u);
  int atUnity = 0;
  int atOrigin = 0;
  for (const auto& z : pz.zeros) {
    if (std::abs(z - std::complex<double>{1.0, 0.0}) < 1e-6) {
      ++atUnity;
    } else if (std::abs(z) < 1e-9) {
      ++atOrigin;
    }
  }
  CHECK(atUnity == 3);
  CHECK(atOrigin == 1);
}

TEST_CASE("PoleZeroTest NotchZerosOnUnitCircleAtCenterFrequency",
          "[filterdesigner]") {
  // scipy.signal.iirnotch places its two numerator zeros exactly on the unit
  // circle at e^{±j w0}, and its poles slightly inside the unit circle at
  // e^{±j w0} * r for some r < 1 determined by Q.
  constexpr double fs = 1000.0;
  constexpr double f0 = 60.0;
  auto filter = SectionsOf(BiquadFilter::Notch(hertz_t{fs}, hertz_t{f0}, 10.0));
  auto pz = ComputePolesZeros(filter);
  REQUIRE(pz.zeros.size() == 2u);
  REQUIRE(pz.poles.size() == 2u);

  // Zeros should lie on the unit circle.
  for (const auto& z : pz.zeros) {
    CHECK_NEAR(std::abs(z), 1.0, 1e-9);
  }
  // Zero angles should be ±2π * f0/fs.
  double expectedAngle = 2.0 * std::numbers::pi * f0 / fs;
  double observedAngle = std::abs(std::arg(pz.zeros.front()));
  CHECK_NEAR(observedAngle, expectedAngle, 1e-6);

  // Poles strictly inside the unit circle, and at the same angle as zeros.
  for (const auto& p : pz.poles) {
    CHECK(std::abs(p) < 1.0);
    CHECK(std::abs(p) > 0.9);  // high-Q notch has poles close to the zeros
  }
  double poleAngle = std::abs(std::arg(pz.poles.front()));
  CHECK_NEAR(poleAngle, expectedAngle, 1e-3);
}

TEST_CASE("PoleZeroTest MovingAverageHasPolesAtOrigin", "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::MovingAverage(5));
  auto pz = ComputePolesZeros(filter);
  // Pure-FIR sections: a1 = a2 = 0 for every section, so every pole is at 0.
  for (const auto& p : pz.poles) {
    CHECK_NEAR(std::abs(p), 0.0, 1e-12);
  }
}

TEST_CASE("PoleZeroTest ComplexPolesAreConjugatePairs", "[filterdesigner]") {
  // Butterworth poles appear in conjugate pairs (for order > 1). Verify by
  // matching each non-real pole to its conjugate.
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 6, 1000_Hz, 100_Hz));
  auto pz = ComputePolesZeros(filter);
  for (const auto& p : pz.poles) {
    if (std::abs(p.imag()) < 1e-12) {
      continue;
    }
    auto conj = std::conj(p);
    bool found = std::any_of(pz.poles.begin(), pz.poles.end(),
                             [&](const std::complex<double>& q) {
                               return std::abs(q - conj) < 1e-9;
                             });
    UNSCOPED_INFO("no conjugate for pole (" << p.real() << ", " << p.imag()
                                            << ")");
    CHECK(found);
  }
}

TEST_CASE("PoleZeroTest TinyGainSectionKeepsItsZeros", "[filterdesigner]") {
  // The cascade gain lands in the first section's numerator. For an 8th-order
  // 1 Hz low-pass at 1 kHz that gain is ~1e-20, so an absolute "is this
  // coefficient zero" cutoff would drop that section's two zeros at z = -1.
  auto filter = SectionsOf(
      BiquadFilter::Butterworth(BiquadFilter::Kind::LowPass, 8, 1000_Hz, 1_Hz));
  REQUIRE(filter.size() == 4u);
  UNSCOPED_INFO("precondition: the gain really is below any fixed cutoff");
  REQUIRE(std::abs(filter[0].b0) < 1e-15);

  auto pz = ComputePolesZeros(filter);
  CHECK(pz.poles.size() == 8u);
  REQUIRE(pz.zeros.size() == 8u);
  for (const auto& z : pz.zeros) {
    UNSCOPED_INFO("zero at (" << z.real() << ", " << z.imag()
                              << ") should sit at -1");
    CHECK_NEAR(z.real(), -1.0, 1e-6);
    CHECK_NEAR(z.imag(), 0.0, 1e-6);
  }
}

TEST_CASE("PoleZeroTest TinyGainSectionKeepsSeparatedZeros",
          "[filterdesigner]") {
  // A band-pass numerator is {g, 0, -g}. Squaring a small enough g underflows,
  // so a discriminant taken on the raw coefficients reads zero and puts both
  // zeros at the origin instead of at ±1.
  Sections sections{Section{1e-200, 0.0, -1e-200, 0.0, 0.0}};
  auto pz = ComputePolesZeros(sections);
  REQUIRE(pz.zeros.size() == 2u);
  std::array<double, 2> real{pz.zeros[0].real(), pz.zeros[1].real()};
  std::ranges::sort(real);
  CHECK_NEAR(real[0], -1.0, 1e-12);
  CHECK_NEAR(real[1], 1.0, 1e-12);
  for (const auto& z : pz.zeros) {
    CHECK_NEAR(z.imag(), 0.0, 1e-12);
  }
}

}  // namespace
