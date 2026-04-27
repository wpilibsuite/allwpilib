// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <cmath>
#include <complex>
#include <numbers>
#include <span>
#include <stdexcept>

#include <gtest/gtest.h>

#include "wpi/math/filter/BiquadFilter.hpp"
#include "wpi/units/frequency.hpp"

namespace {

using wpi::math::BiquadFilter;
using Section = BiquadFilter::Section;
using Kind = BiquadFilter::Kind;

// |H(e^{j·2π·f/fs})| across a cascade of biquad sections.
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
  EXPECT_NEAR(got.b0, want.b0, tol);
  EXPECT_NEAR(got.b1, want.b1, tol);
  EXPECT_NEAR(got.b2, want.b2, tol);
  EXPECT_NEAR(got.a1, want.a1, tol);
  EXPECT_NEAR(got.a2, want.a2, tol);
}

TEST(BiquadFilterDesignTest, ButterworthLowPass4thOrderMatchesScipy) {
  // scipy.signal.butter(4, 50.0, btype='low', fs=1000.0, output='sos')
  auto filter = BiquadFilter::Butterworth(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz);
  auto sections = filter.Sections();
  ASSERT_EQ(sections.size(), 2u);
  ExpectSectionNear(
      sections[0],
      {0.00041659920440659937, 0.0008331984088131987, 0.00041659920440659937,
       -1.4796742169311934, 0.5558215432824889},
      1e-10);
  ExpectSectionNear(sections[1],
                    {1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979},
                    1e-10);
}

TEST(BiquadFilterDesignTest, ButterworthLowPass8thOrderMatchesScipy) {
  // scipy.signal.butter(8, 100.0, btype='low', fs=1000.0, output='sos')
  auto filter =
      BiquadFilter::Butterworth(Kind::LowPass, 8, 1000.0_Hz, 100.0_Hz);
  auto sections = filter.Sections();
  ASSERT_EQ(sections.size(), 4u);
  ExpectSectionNear(
      sections[0],
      {2.3959644103776166e-05, 4.791928820755233e-05, 2.3959644103776166e-05,
       -1.0263514742610553, 0.26864019099379005},
      1e-10);
  ExpectSectionNear(sections[1],
                    {1.0, 2.0, 1.0, -1.0868584613628944, 0.343430940165366},
                    1e-10);
  ExpectSectionNear(sections[2],
                    {1.0, 2.0, 1.0, -1.2197253651240232, 0.5076634651740437},
                    1e-10);
  ExpectSectionNear(sections[3],
                    {1.0, 2.0, 1.0, -1.4515795942478362, 0.794251053241888},
                    1e-10);
}

TEST(BiquadFilterDesignTest, ButterworthLowPassCutoffIsMinusThreeDb) {
  auto filter = BiquadFilter::Butterworth(Kind::LowPass, 4, 1000.0_Hz, 50.0_Hz);
  auto sections = filter.Sections();
  double gainDc = CascadeMagnitude(sections, 0.0, 1000.0);
  double gainFc = CascadeMagnitude(sections, 50.0, 1000.0);
  EXPECT_NEAR(gainDc, 1.0, 1e-10);
  EXPECT_NEAR(20.0 * std::log10(gainFc), -3.0, 0.05);
}

TEST(BiquadFilterDesignTest, ButterworthHighPassResponse) {
  auto filter =
      BiquadFilter::Butterworth(Kind::HighPass, 4, 1000.0_Hz, 100.0_Hz);
  auto sections = filter.Sections();
  double gainDc = CascadeMagnitude(sections, 0.0, 1000.0);
  double gainFc = CascadeMagnitude(sections, 100.0, 1000.0);
  double gainHigh = CascadeMagnitude(sections, 400.0, 1000.0);
  EXPECT_NEAR(gainDc, 0.0, 1e-10);
  EXPECT_NEAR(20.0 * std::log10(gainFc), -3.0, 0.05);
  EXPECT_NEAR(gainHigh, 1.0, 1e-3);
}

TEST(BiquadFilterDesignTest, ButterworthBandPass4thOrderMatchesScipy) {
  // scipy.signal.butter(4, [80.0, 120.0], btype='bandpass', fs=1000.0)
  auto filter = BiquadFilter::Butterworth(Kind::BandPass, 4, 1000.0_Hz, 80.0_Hz,
                                          120.0_Hz);
  auto sections = filter.Sections();
  ASSERT_EQ(sections.size(), 4u);
  ExpectSectionNear(
      sections[0],
      {0.0001832160233696091, 0.0003664320467392182, 0.0001832160233696091,
       -1.395944592254935, 0.7785762494967928},
      1e-10);
  ExpectSectionNear(sections[1],
                    {1.0, 2.0, 1.0, -1.5194742571654707, 0.8044610397041421},
                    1e-10);
  ExpectSectionNear(sections[2],
                    {1.0, -2.0, 1.0, -1.395095159020637, 0.8950130915917338},
                    1e-10);
  ExpectSectionNear(sections[3],
                    {1.0, -2.0, 1.0, -1.678184355447092, 0.9231164780821922},
                    1e-10);
}

TEST(BiquadFilterDesignTest, ButterworthBandPassResponse) {
  auto filter = BiquadFilter::Butterworth(Kind::BandPass, 4, 1000.0_Hz, 80.0_Hz,
                                          120.0_Hz);
  auto sections = filter.Sections();
  double gainDc = CascadeMagnitude(sections, 0.0, 1000.0);
  double gainCenter = CascadeMagnitude(sections, 100.0, 1000.0);
  double gainNyquist = CascadeMagnitude(sections, 499.0, 1000.0);
  EXPECT_LT(gainDc, 1e-6);
  EXPECT_LT(gainNyquist, 1e-6);
  EXPECT_GT(gainCenter, 0.8);
}

TEST(BiquadFilterDesignTest, ButterworthBandStopResponse) {
  auto filter = BiquadFilter::Butterworth(Kind::BandStop, 4, 1000.0_Hz, 80.0_Hz,
                                          120.0_Hz);
  auto sections = filter.Sections();
  double gainDc = CascadeMagnitude(sections, 0.0, 1000.0);
  // Geometric mean is the analog-prototype zero; digital zero is nearby.
  double gainCenter =
      CascadeMagnitude(sections, std::sqrt(80.0 * 120.0), 1000.0);
  double gainNyquist = CascadeMagnitude(sections, 499.0, 1000.0);
  EXPECT_NEAR(gainDc, 1.0, 1e-3);
  EXPECT_NEAR(gainNyquist, 1.0, 1e-3);
  EXPECT_LT(gainCenter, 1e-6);
}

TEST(BiquadFilterDesignTest, ButterworthRejectsInvalidArgs) {
  EXPECT_THROW(BiquadFilter::Butterworth(Kind::LowPass, 0, 1000.0_Hz, 50.0_Hz),
               std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Butterworth(Kind::LowPass, 4, 0.0_Hz, 50.0_Hz),
               std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Butterworth(Kind::LowPass, 4, 1000.0_Hz, 0.0_Hz),
               std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Butterworth(Kind::LowPass, 4, 1000.0_Hz, 500.0_Hz),
               std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Butterworth(Kind::LowPass, 4, 1000.0_Hz, 600.0_Hz),
               std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Butterworth(Kind::BandPass, 4, 1000.0_Hz, 120.0_Hz,
                                         80.0_Hz),
               std::invalid_argument);
  EXPECT_THROW(
      BiquadFilter::Butterworth(Kind::BandPass, 4, 1000.0_Hz, 80.0_Hz, 80.0_Hz),
      std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Butterworth(Kind::BandStop, 4, 1000.0_Hz, 80.0_Hz,
                                         500.0_Hz),
               std::invalid_argument);
}

TEST(BiquadFilterDesignTest, Notch60HzMatchesScipy) {
  // scipy.signal.iirnotch(60.0, Q=10.0, fs=1000.0)
  auto filter = BiquadFilter::Notch(1000.0_Hz, 60.0_Hz, 10.0);
  auto sections = filter.Sections();
  ASSERT_EQ(sections.size(), 1u);
  ExpectSectionNear(
      sections[0],
      {0.9814970254751076, -1.8251457105120343, 0.9814970254751076,
       -1.8251457105120341, 0.9629940509502151},
      1e-12);
}

TEST(BiquadFilterDesignTest, NotchAttenuatesAtCenter) {
  auto filter = BiquadFilter::Notch(1000.0_Hz, 60.0_Hz, 10.0);
  auto sections = filter.Sections();
  double gainDc = CascadeMagnitude(sections, 0.0, 1000.0);
  double gainNotch = CascadeMagnitude(sections, 60.0, 1000.0);
  double gainFar = CascadeMagnitude(sections, 200.0, 1000.0);
  EXPECT_NEAR(gainDc, 1.0, 1e-6);
  EXPECT_LT(gainNotch, 1e-6);
  EXPECT_NEAR(gainFar, 1.0, 0.05);
}

TEST(BiquadFilterDesignTest, NotchRejectsInvalidArgs) {
  EXPECT_THROW(BiquadFilter::Notch(0.0_Hz, 60.0_Hz, 10.0),
               std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Notch(-1000.0_Hz, 60.0_Hz, 10.0),
               std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Notch(1000.0_Hz, 0.0_Hz, 10.0),
               std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Notch(1000.0_Hz, 500.0_Hz, 10.0),
               std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Notch(1000.0_Hz, 600.0_Hz, 10.0),
               std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Notch(1000.0_Hz, 60.0_Hz, 0.0),
               std::invalid_argument);
  EXPECT_THROW(BiquadFilter::Notch(1000.0_Hz, 60.0_Hz, -1.0),
               std::invalid_argument);
}

TEST(BiquadFilterDesignTest, MovingAverageSingleTapIsPassThrough) {
  auto filter = BiquadFilter::MovingAverage(1);
  auto sections = filter.Sections();
  ASSERT_EQ(sections.size(), 1u);
  ExpectSectionNear(sections[0], {1.0, 0.0, 0.0, 0.0, 0.0}, 0.0);
}

TEST(BiquadFilterDesignTest,
     MovingAverageEvenLengthHasUnitDcGainAndNyquistNull) {
  auto filter = BiquadFilter::MovingAverage(4);
  auto sections = filter.Sections();
  double gainDc = CascadeMagnitude(sections, 0.0, 1000.0);
  double gainNyquist = CascadeMagnitude(sections, 500.0, 1000.0);
  EXPECT_NEAR(gainDc, 1.0, 1e-12);
  EXPECT_LT(gainNyquist, 1e-12);
}

TEST(BiquadFilterDesignTest, MovingAverageOddLengthNullsAtFsOverN) {
  constexpr double kFs = 1000.0;
  constexpr int kN = 5;
  auto filter = BiquadFilter::MovingAverage(kN);
  auto sections = filter.Sections();
  double gainDc = CascadeMagnitude(sections, 0.0, kFs);
  double gainNull = CascadeMagnitude(sections, kFs / kN, kFs);
  double gainHalfNull = CascadeMagnitude(sections, kFs / (2.0 * kN), kFs);
  EXPECT_NEAR(gainDc, 1.0, 1e-12);
  EXPECT_LT(gainNull, 1e-10);
  EXPECT_GT(gainHalfNull, 0.1);
}

TEST(BiquadFilterDesignTest, MovingAverageMatchesSumAverageImpulseResponse) {
  // Verify impulse response of the cascade equals 1/N for N samples, then 0.
  constexpr int kN = 7;
  auto filter = BiquadFilter::MovingAverage(kN);

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

TEST(BiquadFilterDesignTest, MovingAverageRejectsInvalidArgs) {
  EXPECT_THROW(BiquadFilter::MovingAverage(0), std::invalid_argument);
  EXPECT_THROW(BiquadFilter::MovingAverage(-1), std::invalid_argument);
}

}  // namespace
