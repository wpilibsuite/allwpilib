// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/PoleZero.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <numbers>

#include <gtest/gtest.h>

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

TEST(PoleZeroTest, EmptyCascadeHasNoRoots) {
  Sections empty;
  auto pz = ComputePolesZeros(empty);
  EXPECT_TRUE(pz.poles.empty());
  EXPECT_TRUE(pz.zeros.empty());
}

TEST(PoleZeroTest, PassThroughHasPolesAndZerosAtOrigin) {
  // b0=1, everything else 0 — H(z) = 1. The quadratic-as-stored form is
  // 1*z^2 + 0*z + 0, root at 0 (double), same on the denominator (a1=a2=0).
  Sections s{Section{1.0, 0.0, 0.0, 0.0, 0.0}};
  auto pz = ComputePolesZeros(s);
  ASSERT_EQ(pz.poles.size(), 2u);
  ASSERT_EQ(pz.zeros.size(), 2u);
  for (const auto& p : pz.poles) {
    EXPECT_NEAR(std::abs(p), 0.0, 1e-12);
  }
  for (const auto& z : pz.zeros) {
    EXPECT_NEAR(std::abs(z), 0.0, 1e-12);
  }
}

TEST(PoleZeroTest, ButterworthLowPassPolesInsideUnitCircle) {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  auto pz = ComputePolesZeros(filter);
  // 2 biquad sections × 2 poles = 4 poles.
  EXPECT_EQ(pz.poles.size(), 4u);
  // Stability: every pole strictly inside the unit circle.
  for (const auto& p : pz.poles) {
    EXPECT_LT(std::abs(p), 1.0 - 1e-6)
        << "pole at (" << p.real() << ", " << p.imag()
        << ") not inside unit circle";
  }
}

TEST(PoleZeroTest, ButterworthLowPassZerosAtNyquist) {
  // The bilinear transform maps analog zeros at infinity to z = -1 (Nyquist).
  // A 4th-order Butterworth LP has 4 zeros, all at z = -1. Tolerance is ~1e-6
  // because (a) the design pipeline accumulates small floating-point error and
  // (b) repeated real roots come out of the quadratic formula with a spurious
  // imaginary part on the order of sqrt(machine epsilon) when the discriminant
  // is computed as a near-zero difference.
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  auto pz = ComputePolesZeros(filter);
  ASSERT_EQ(pz.zeros.size(), 4u);
  for (const auto& z : pz.zeros) {
    EXPECT_LT(std::abs(z - std::complex<double>{-1.0, 0.0}), 1e-6)
        << "zero at (" << z.real() << ", " << z.imag() << ") not near Nyquist";
  }
}

TEST(PoleZeroTest, ButterworthHighPassZerosAtDC) {
  // High-pass: analog zeros at 0 map to z = 1 under bilinear transform. For
  // order=3, the cascade is one biquad (real-pole-pair) + one first-order
  // section. The biquad's numerator is (z-1)^2 (two zeros at unity); the
  // first-order section's biquad form is z(z-1), contributing one more zero
  // at unity plus a structural zero at the origin.
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::HighPass, 3, 1000_Hz, 100_Hz));
  auto pz = ComputePolesZeros(filter);
  ASSERT_EQ(pz.zeros.size(), 4u);
  int atUnity = 0;
  int atOrigin = 0;
  for (const auto& z : pz.zeros) {
    if (std::abs(z - std::complex<double>{1.0, 0.0}) < 1e-6) {
      ++atUnity;
    } else if (std::abs(z) < 1e-9) {
      ++atOrigin;
    }
  }
  EXPECT_EQ(atUnity, 3);
  EXPECT_EQ(atOrigin, 1);
}

TEST(PoleZeroTest, NotchZerosOnUnitCircleAtCenterFrequency) {
  // scipy.signal.iirnotch places its two numerator zeros exactly on the unit
  // circle at e^{±j w0}, and its poles slightly inside the unit circle at
  // e^{±j w0} * r for some r < 1 determined by Q.
  constexpr double fs = 1000.0;
  constexpr double f0 = 60.0;
  auto filter = SectionsOf(BiquadFilter::Notch(hertz_t{fs}, hertz_t{f0}, 10.0));
  auto pz = ComputePolesZeros(filter);
  ASSERT_EQ(pz.zeros.size(), 2u);
  ASSERT_EQ(pz.poles.size(), 2u);

  // Zeros should lie on the unit circle.
  for (const auto& z : pz.zeros) {
    EXPECT_NEAR(std::abs(z), 1.0, 1e-9);
  }
  // Zero angles should be ±2π * f0/fs.
  double expectedAngle = 2.0 * std::numbers::pi * f0 / fs;
  double observedAngle = std::abs(std::arg(pz.zeros.front()));
  EXPECT_NEAR(observedAngle, expectedAngle, 1e-6);

  // Poles strictly inside the unit circle, and at the same angle as zeros.
  for (const auto& p : pz.poles) {
    EXPECT_LT(std::abs(p), 1.0);
    EXPECT_GT(std::abs(p), 0.9);  // high-Q notch has poles close to the zeros
  }
  double poleAngle = std::abs(std::arg(pz.poles.front()));
  EXPECT_NEAR(poleAngle, expectedAngle, 1e-3);
}

TEST(PoleZeroTest, MovingAverageHasPolesAtOrigin) {
  auto filter = SectionsOf(BiquadFilter::MovingAverage(5));
  auto pz = ComputePolesZeros(filter);
  // Pure-FIR sections: a1 = a2 = 0 for every section, so every pole is at 0.
  for (const auto& p : pz.poles) {
    EXPECT_NEAR(std::abs(p), 0.0, 1e-12);
  }
}

TEST(PoleZeroTest, ComplexPolesAreConjugatePairs) {
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
    EXPECT_TRUE(found) << "no conjugate for pole (" << p.real() << ", "
                       << p.imag() << ")";
  }
}

}  // namespace
