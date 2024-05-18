// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/MathExtras.h>
#include <wpi/timestamp.h>

#include "frc/LEDPattern.h"

namespace frc {

static units::second_t now = 0_s;

class LEDPatternTest : public ::testing::Test {
 protected:
  void SetUp() override {
    WPI_SetNowImpl([] { return units::microsecond_t{now}.to<uint64_t>(); });
  }

  void TearDown() override { WPI_SetNowImpl(nullptr); }
};

void AssertIndexColor(std::span<AddressableLED::LEDData> data, int index,
                      Color color);
Color LerpColors(Color a, Color b, double t);

TEST(LEDPatternTest, SolidColor) {
  LEDPattern pattern = LEDPattern::Solid(Color::kYellow);
  std::array<AddressableLED::LEDData, 5> buffer;

  // prefill
  for (int i = 0; i < 5; i++) {
    buffer[i].SetLED(Color::kPurple);
  }

  pattern.ApplyTo(buffer);
  for (int i = 0; i < 5; i++) {
    AssertIndexColor(buffer, i, Color::kYellow);
  }
}

TEST(LEDPatternTest, EmptyGradientSetsToBlack) {
  std::array<Color, 0> colors;
  LEDPattern pattern = LEDPattern::Gradient(colors);
  std::array<AddressableLED::LEDData, 5> buffer;
  pattern.ApplyTo(buffer);
  for (int i = 0; i < 5; i++) {
    AssertIndexColor(buffer, i, Color::kBlack);
  }
}

TEST(LEDPatternTest, SingleColorGradientSetsSolid) {
  std::array<Color, 1> colors{Color::kYellow};
  LEDPattern pattern = LEDPattern::Gradient(colors);
  std::array<AddressableLED::LEDData, 5> buffer;
  pattern.ApplyTo(buffer);
  for (int i = 0; i < 5; i++) {
    AssertIndexColor(buffer, i, Color::kYellow);
  }
}

TEST(LEDPatternTest, Gradient2Colors) {
  std::array<Color, 2> colors{Color::kYellow, Color::kPurple};
  LEDPattern pattern = LEDPattern::Gradient(colors);
  std::array<AddressableLED::LEDData, 99> buffer;
  pattern.ApplyTo(buffer);
  AssertIndexColor(buffer, 0, Color::kYellow);
  AssertIndexColor(buffer, 25,
                   LerpColors(Color::kYellow, Color::kPurple, 25 / 49.0));
  AssertIndexColor(buffer, 49, Color::kPurple);
  AssertIndexColor(buffer, 74,
                   LerpColors(Color::kPurple, Color::kYellow, 25 / 49.0));
  AssertIndexColor(buffer, 98, Color::kYellow);
}

TEST(LEDPatternTest, Gradient3Colors) {
  std::array<Color, 3> colors{Color::kYellow, Color::kPurple, Color::kWhite};
  LEDPattern pattern = LEDPattern::Gradient(colors);
  std::array<AddressableLED::LEDData, 99> buffer;
  pattern.ApplyTo(buffer);

  AssertIndexColor(buffer, 0, Color::kYellow);
  AssertIndexColor(buffer, 25,
                   LerpColors(Color::kYellow, Color::kPurple, 25 / 33.0));
  AssertIndexColor(buffer, 33, Color::kPurple);
  AssertIndexColor(buffer, 58,
                   LerpColors(Color::kPurple, Color::kWhite, 25 / 33.0));
  AssertIndexColor(buffer, 66, Color::kWhite);
  AssertIndexColor(buffer, 91,
                   LerpColors(Color::kWhite, Color::kYellow, 25 / 33.0));
  AssertIndexColor(buffer, 98,
                   LerpColors(Color::kWhite, Color::kYellow, 32 / 33.0));
}

TEST(LEDPatternTest, EmptyStepsSetsToBlack) {
  std::array<std::pair<double, Color>, 0> steps;
  LEDPattern pattern = LEDPattern::Steps(steps);
  std::array<AddressableLED::LEDData, 5> buffer;

  // prefill
  for (int i = 0; i < 5; i++) {
    buffer[i].SetLED(Color::kPurple);
  }

  pattern.ApplyTo(buffer);

  for (int i = 0; i < 5; i++) {
    AssertIndexColor(buffer, i, Color::kBlack);
  }
}

TEST(LEDPatternTest, SingleStepSetsSolid) {
  std::array<std::pair<double, Color>, 1> steps{std::pair{0.0, Color::kYellow}};
  LEDPattern pattern = LEDPattern::Steps(steps);
  std::array<AddressableLED::LEDData, 5> buffer;

  pattern.ApplyTo(buffer);

  for (int i = 0; i < 5; i++) {
    AssertIndexColor(buffer, i, Color::kYellow);
  }
}

// TEST(LEDPatternTest, SingleHalfStepSetsHalfOffHalfColor) {
//   std::array<std::pair<double, Color>, 1> steps{std::pair{0.5,
//   Color::kYellow}}; LEDPattern pattern = LEDPattern::Steps(steps);
//   std::array<AddressableLED::LEDData, 99> buffer;

//   pattern.ApplyTo(buffer);

//   // [0, 48] should be black...
//   for (int i = 0; i < 49; i++) {
//     AssertIndexColor(buffer, i, Color::kBlack);
//   }

//   // ... and [49, <end>] should be the color that was set
//   for (int i = 49; i < 99; i++) {
//     AssertIndexColor(buffer, i, Color::kYellow);
//   }
// }

TEST(LEDPatternTest, ScrollRelativeForward) {
  // A black to white gradient
  LEDPattern pattern = LEDPattern{[=](auto data, auto writer) {
    for (size_t led = 0; led < data.size(); led++) {
      int ch = static_cast<int>(led % 256);
      writer(led, Color{ch, ch, ch});
    }
  }};
  std::array<AddressableLED::LEDData, 256> buffer;

  // Scrolling at 1/256th of the buffer per second,
  // or 1 individual diode per second
  auto scroll = pattern.ScrollAtRelativeSpeed(1 / 256.0);

  static uint64_t now = 0ull;
  WPI_SetNowImpl([] { return now; });

  for (int time = 0; time < 500; time++) {
    // convert time (seconds) to microseconds
    now = time * 1000000ull;

    scroll.ApplyTo(buffer);

    for (size_t led = 0; led < buffer.size(); led++) {
      SCOPED_TRACE(
          fmt::format("LED {} of 256, run {} of 500", led + 1, time + 1));
      // Base: [(0, 0, 0) (1, 1, 1) (2, 2, 2) (3, 3, 3) (4, 4, 4) ... (255, 255,
      // 255)] Value for every channel should DECREASE by 1 in each timestep,
      // wrapping around 0 and 255

      // t=0,   channel value = (0, 1, 2, ..., 254, 255)
      // t=1,   channel value = (255, 0, 1, ..., 253, 254)
      // t=2,   channel value = (254, 255, 0, ..., 252, 253)
      // t=255, channel value = (1, 2, 3, ..., 255, 0)
      // t=256, channel value = (0, 1, 2, ..., 254, 255)
      int ch = LEDPattern::floorMod(led - time, 256);
      AssertIndexColor(buffer, led, Color{ch, ch, ch});
    }
  }

  WPI_SetNowImpl(nullptr);  // cleanup
}

TEST(LEDPatternTest, ScrollRelativeBackward) {
  // A black to white gradient
  LEDPattern pattern = LEDPattern{[=](auto data, auto writer) {
    for (size_t led = 0; led < data.size(); led++) {
      int ch = static_cast<int>(led % 256);
      writer(led, Color{ch, ch, ch});
    }
  }};
  std::array<AddressableLED::LEDData, 256> buffer;

  // Scrolling at 1/256th of the buffer per second,
  // or 1 individual diode per second
  auto scroll = pattern.ScrollAtRelativeSpeed(-1 / 256.0);

  static uint64_t now = 0ull;
  WPI_SetNowImpl([] { return now; });

  for (int time = 0; time < 500; time++) {
    // convert time (seconds) to microseconds
    now = time * 1000000ull;

    scroll.ApplyTo(buffer);

    for (size_t led = 0; led < buffer.size(); led++) {
      SCOPED_TRACE(
          fmt::format("LED {} of 256, run {} of 500", led + 1, time + 1));
      // Base: [(0, 0, 0) (1, 1, 1) (2, 2, 2) (3, 3, 3) (4, 4, 4) ... (255, 255,
      // 255)] Value for every channel should DECREASE by 1 in each timestep,
      // wrapping around 0 and 255

      // t=0,   channel value = (0, 1, 2, ..., 254, 255)
      // t=1,   channel value = (255, 0, 1, ..., 253, 254)
      // t=2,   channel value = (254, 255, 0, ..., 252, 253)
      // t=255, channel value = (1, 2, 3, ..., 255, 0)
      // t=256, channel value = (0, 1, 2, ..., 254, 255)
      int ch = LEDPattern::floorMod(led + time, 256);
      AssertIndexColor(buffer, led, Color{ch, ch, ch});
    }
  }

  WPI_SetNowImpl(nullptr);  // cleanup
}

TEST(LEDPatternTest, ScrollAbsoluteForward) {}
TEST(LEDPatternTest, ScrollAbsoluteBackward) {}
TEST(LEDPatternTest, Rainbow) {}
TEST(LEDPatternTest, ReverseSolid) {}
TEST(LEDPatternTest, ReverseSteps) {}
TEST(LEDPatternTest, OffsetPositive) {}
TEST(LEDPatternTest, OffsetNegative) {}
TEST(LEDPatternTest, OffsetZero) {}
TEST(LEDPatternTest, BlinkSymmetric) {}
TEST(LEDPatternTest, BlinkAsymmetric) {}
TEST(LEDPatternTest, BlinkInSync) {}
TEST(LEDPatternTest, Breate) {}
TEST(LEDPatternTest, OverlaySolidOnSolid) {}
TEST(LEDPatternTest, OverlayNearlyBlack) {}
TEST(LEDPatternTest, OverlayMixed) {}
TEST(LEDPatternTest, Blend) {}
TEST(LEDPatternTest, BinaryMask) {}
TEST(LEDPatternTest, ChannelwiseMask) {}
TEST(LEDPatternTest, ProcessMaskLayer) {}
TEST(LEDPatternTest, ZeroBrightness) {}
TEST(LEDPatternTest, SameBrightness) {}
TEST(LEDPatternTest, HigherBrightness) {}
TEST(LEDPatternTest, NegativeBrightness) {}
TEST(LEDPatternTest, ClippingBrightness) {}

void AssertIndexColor(std::span<AddressableLED::LEDData> data, int index,
                      Color color) {
  frc::Color8Bit color8bit{color};

  EXPECT_EQ(0, data[index].padding);
  EXPECT_EQ(color8bit.red, data[index].r & 0xFF);
  EXPECT_EQ(color8bit.green, data[index].g & 0xFF);
  EXPECT_EQ(color8bit.blue, data[index].b & 0xFF);
}

Color LerpColors(Color a, Color b, double t) {
  return Color{wpi::Lerp(a.red, b.red, t), wpi::Lerp(a.green, b.green, t),
               wpi::Lerp(a.blue, b.blue, t)};
}
}  // namespace frc
