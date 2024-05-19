// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/MathExtras.h>
#include <wpi/timestamp.h>

#include "frc/LEDPattern.h"

namespace frc {

static LEDPattern whiteYellowPurple{[](auto data, auto writer) {
  for (size_t led = 0; led < data.size(); led++) {
    switch (led % 3) {
      case 0:
        writer(led, Color::kWhite);
        break;
      case 1:
        writer(led, Color::kYellow);
        break;
      case 2:
        writer(led, Color::kPurple);
        break;
    }
  }
}};

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

TEST(LEDPatternTest, SingleHalfStepSetsHalfOffHalfColor) {
  std::array<std::pair<double, Color>, 1> steps{std::pair{0.5, Color::kYellow}};
  LEDPattern pattern = LEDPattern::Steps(steps);
  std::array<AddressableLED::LEDData, 99> buffer;

  pattern.ApplyTo(buffer);

  // [0, 48] should be black...
  for (int i = 0; i < 49; i++) {
    AssertIndexColor(buffer, i, Color::kBlack);
  }

  // ... and [49, <end>] should be the color that was set
  for (int i = 49; i < 99; i++) {
    AssertIndexColor(buffer, i, Color::kYellow);
  }
}

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

TEST(LEDPatternTest, ScrollAbsoluteForward) {
  // A black to white gradient
  LEDPattern pattern = LEDPattern{[](auto data, auto writer) {
    for (size_t led = 0; led < data.size(); led++) {
      int ch = static_cast<int>(led % 256);
      writer(led, Color{ch, ch, ch});
    }
  }};
  std::array<AddressableLED::LEDData, 256> buffer;
  // scroll at 16 m/s, LED spacing = 2cm
  // buffer is 256 LEDs, so total length = 512cm = 5.12m
  // scrolling at 16 m/s yields a period of 0.32 seconds,
  // or 0.00125 seconds per LED (800 LEDs/s)
  auto scroll = pattern.ScrollAtAbsoluteSpeed(16_mps, 2_cm);

  static uint64_t now = 0ull;
  WPI_SetNowImpl([] { return now; });

  for (int time = 0; time < 500; time++) {
    // convert time (seconds) to microseconds
    now = time * 1250ull;  // 1.25ms per LED

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

TEST(LEDPatternTest, ScrollAbsoluteBackward) {
  // A black to white gradient
  LEDPattern pattern = LEDPattern{[](auto data, auto writer) {
    for (size_t led = 0; led < data.size(); led++) {
      int ch = static_cast<int>(led % 256);
      writer(led, Color{ch, ch, ch});
    }
  }};
  std::array<AddressableLED::LEDData, 256> buffer;
  // scroll at 16 m/s, LED spacing = 2cm
  // buffer is 256 LEDs, so total length = 512cm = 5.12m
  // scrolling at 16 m/s yields a period of 0.32 seconds,
  // or 0.00125 seconds per LED (800 LEDs/s)
  auto scroll = pattern.ScrollAtAbsoluteSpeed(-16_mps, 2_cm);

  static uint64_t now = 0ull;
  WPI_SetNowImpl([] { return now; });

  for (int time = 0; time < 500; time++) {
    // convert time (seconds) to microseconds
    now = time * 1250ull;  // 1.25ms per LED

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

TEST(LEDPatternTest, RainbowFullSize) {
  std::array<AddressableLED::LEDData, 180> buffer;
  int saturation = 255;
  int value = 255;
  LEDPattern pattern = LEDPattern::Rainbow(saturation, value);
  pattern.ApplyTo(buffer);

  for (int led = 0; led < 180; led++) {
    AssertIndexColor(buffer, led, Color::FromHSV(led, saturation, value));
  }
}

TEST(LEDPatternTest, RainbowHalfSize) {
  std::array<AddressableLED::LEDData, 90> buffer;
  int saturation = 42;
  int value = 87;
  LEDPattern pattern = LEDPattern::Rainbow(saturation, value);
  pattern.ApplyTo(buffer);

  for (int led = 0; led < 90; led++) {
    AssertIndexColor(buffer, led, Color::FromHSV(led * 2, saturation, value));
  }
}

TEST(LEDPatternTest, RainbowThirdSize) {
  std::array<AddressableLED::LEDData, 60> buffer;
  int saturation = 191;
  int value = 255;
  LEDPattern pattern = LEDPattern::Rainbow(saturation, value);
  pattern.ApplyTo(buffer);

  for (int led = 0; led < 60; led++) {
    SCOPED_TRACE(fmt::format("LED {} of 60", led + 1));
    AssertIndexColor(buffer, led, Color::FromHSV(led * 3, saturation, value));
  }
}

TEST(LEDPatternTest, RainbowDoubleSize) {
  std::array<AddressableLED::LEDData, 360> buffer;
  int saturation = 212;
  int value = 93;
  LEDPattern pattern = LEDPattern::Rainbow(saturation, value);
  pattern.ApplyTo(buffer);

  for (int led = 0; led < 360; led++) {
    SCOPED_TRACE(fmt::format("LED {} of 360", led + 1));
    AssertIndexColor(buffer, led, Color::FromHSV(led / 2, saturation, value));
  }
}

TEST(LEDPatternTest, RainbowOddSize) {
  std::array<AddressableLED::LEDData, 127> buffer;
  double scale = 180.0 / 127;
  int saturation = 73;
  int value = 128;
  LEDPattern pattern = LEDPattern::Rainbow(saturation, value);
  pattern.ApplyTo(buffer);

  for (int led = 0; led < 127; led++) {
    SCOPED_TRACE(fmt::format("LED {} of 127", led + 1));
    AssertIndexColor(
        buffer, led,
        Color::FromHSV(static_cast<int>(led * scale), saturation, value));
  }
}

TEST(LEDPatternTest, ReverseSolid) {
  std::array<AddressableLED::LEDData, 90> buffer;
  const auto color = Color::kRosyBrown;

  auto solid = LEDPattern::Solid(color);
  auto pattern = solid.Reversed();

  pattern.ApplyTo(buffer);

  for (int led = 0; led < 90; led++) {
    SCOPED_TRACE(fmt::format("LED {} of 90", led + 1));
    AssertIndexColor(buffer, led, Color::kRosyBrown);
  }
}

TEST(LEDPatternTest, ReverseSteps) {
  std::array<AddressableLED::LEDData, 100> buffer;
  std::array<std::pair<double, Color>, 2> steps{std::pair{0.0, Color::kPlum},
                                                std::pair{0.5, Color::kYellow}};
  auto stepPattern = LEDPattern::Steps(steps);
  auto pattern = stepPattern.Reversed();

  pattern.ApplyTo(buffer);

  // colors should be swapped; yellow first, then plum
  for (int led = 0; led < 50; led++) {
    SCOPED_TRACE(fmt::format("LED {} of 100", led + 1));
    AssertIndexColor(buffer, led, Color::kYellow);
  }
  for (int led = 50; led < 100; led++) {
    SCOPED_TRACE(fmt::format("LED {} of 100", led + 1));
    AssertIndexColor(buffer, led, Color::kPlum);
  }
}

TEST(LEDPatternTest, OffsetPositive) {
  std::array<AddressableLED::LEDData, 21> buffer;
  auto offset = whiteYellowPurple.OffsetBy(1);
  offset.ApplyTo(buffer);

  for (int led = 0; led < 21; led++) {
    SCOPED_TRACE(fmt::format("LED {} of 21", led + 1));
    switch (led % 3) {
      case 0:
        AssertIndexColor(buffer, led, Color::kPurple);
        break;
      case 1:
        AssertIndexColor(buffer, led, Color::kWhite);
        break;
      case 2:
        AssertIndexColor(buffer, led, Color::kYellow);
        break;
    }
  }
}

TEST(LEDPatternTest, OffsetNegative) {
  std::array<AddressableLED::LEDData, 21> buffer;
  auto offset = whiteYellowPurple.OffsetBy(-1);
  offset.ApplyTo(buffer);

  for (int led = 0; led < 21; led++) {
    SCOPED_TRACE(fmt::format("LED {} of 21", led + 1));
    switch (led % 3) {
      case 0:
        AssertIndexColor(buffer, led, Color::kYellow);
        break;
      case 1:
        AssertIndexColor(buffer, led, Color::kPurple);
        break;
      case 2:
        AssertIndexColor(buffer, led, Color::kWhite);
        break;
    }
  }
}

TEST(LEDPatternTest, OffsetZero) {
  std::array<AddressableLED::LEDData, 21> buffer;
  auto offset = whiteYellowPurple.OffsetBy(0);
  offset.ApplyTo(buffer);

  for (int led = 0; led < 21; led++) {
    SCOPED_TRACE(fmt::format("LED {} of 21", led + 1));
    switch (led % 3) {
      case 0:
        AssertIndexColor(buffer, led, Color::kWhite);
        break;
      case 1:
        AssertIndexColor(buffer, led, Color::kYellow);
        break;
      case 2:
        AssertIndexColor(buffer, led, Color::kPurple);
        break;
    }
  }
}

TEST(LEDPatternTest, BlinkSymmetric) {
  std::array<AddressableLED::LEDData, 1> buffer;
  auto white = LEDPattern::Solid(Color::kWhite);

  // on for 2 seconds, off for 2 seconds
  auto pattern = white.Blink(2_s);

  static uint64_t now = 0ull;
  WPI_SetNowImpl([] { return now; });
  for (int t = 0; t < 8; t++) {
    now = t * 1000000ull;  // time travel 1 second
    SCOPED_TRACE(fmt::format("Time {} seconds", t));
    pattern.ApplyTo(buffer);

    switch (t) {
      case 0:
      case 1:
      case 4:
      case 5:
        AssertIndexColor(buffer, 0, Color::kWhite);
        break;
      case 2:
      case 3:
      case 6:
      case 7:
        AssertIndexColor(buffer, 0, Color::kBlack);
        break;
    }
  }

  WPI_SetNowImpl(nullptr);  // cleanup
}

TEST(LEDPatternTest, BlinkAsymmetric) {
  std::array<AddressableLED::LEDData, 1> buffer;
  auto white = LEDPattern::Solid(Color::kWhite);

  // on for 3 seconds, off for 1 second
  auto pattern = white.Blink(3_s, 1_s);

  static uint64_t now = 0ull;
  WPI_SetNowImpl([] { return now; });
  for (int t = 0; t < 8; t++) {
    now = t * 1000000ull;  // time travel 1 second
    SCOPED_TRACE(fmt::format("Time {} seconds", t));
    pattern.ApplyTo(buffer);

    switch (t) {
      case 0:
      case 1:
      case 2:  // first period
      case 4:
      case 5:
      case 6:  // second period
        AssertIndexColor(buffer, 0, Color::kWhite);
        break;
      case 3:
      case 7:
        AssertIndexColor(buffer, 0, Color::kBlack);
        break;
    }
  }

  WPI_SetNowImpl(nullptr);  // cleanup
}

TEST(LEDPatternTest, BlinkInSync) {
  std::array<AddressableLED::LEDData, 1> buffer;
  auto white = LEDPattern::Solid(Color::kWhite);

  bool flag = false;
  auto condition = [&flag]() { return flag; };

  auto pattern = white.SynchronizedBlink(condition);

  SCOPED_TRACE("Flag off");
  pattern.ApplyTo(buffer);
  AssertIndexColor(buffer, 0, Color::kBlack);

  SCOPED_TRACE("Flag on");
  flag = true;
  pattern.ApplyTo(buffer);
  AssertIndexColor(buffer, 0, Color::kWhite);

  SCOPED_TRACE("Flag off");
  flag = false;
  pattern.ApplyTo(buffer);
  AssertIndexColor(buffer, 0, Color::kBlack);
}

TEST(LEDPatternTest, Breathe) {
  Color midGray{0.5, 0.5, 0.5};
  std::array<AddressableLED::LEDData, 1> buffer;
  auto white = LEDPattern::Solid(Color::kWhite);
  auto pattern = white.Breathe(4_us);

  static uint64_t now = 0ull;
  WPI_SetNowImpl([] { return now; });

  {
    now = 0ull;  // start
    SCOPED_TRACE(fmt::format("Time {}", now));

    pattern.ApplyTo(buffer);
    AssertIndexColor(buffer, 0, Color::kWhite);
  }
  {
    now = 1ull;  // midway (down)
    SCOPED_TRACE(fmt::format("Time {}", now));

    pattern.ApplyTo(buffer);
    AssertIndexColor(buffer, 0, midGray);
  }
  {
    now = 2ull;  // bottom
    SCOPED_TRACE(fmt::format("Time {}", now));

    pattern.ApplyTo(buffer);
    AssertIndexColor(buffer, 0, Color::kBlack);
  }
  {
    now = 3ull;  // midway (up)
    SCOPED_TRACE(fmt::format("Time {}", now));

    pattern.ApplyTo(buffer);
    AssertIndexColor(buffer, 0, midGray);
  }
  {
    now = 4ull;  // back to start
    SCOPED_TRACE(fmt::format("Time {}", now));

    pattern.ApplyTo(buffer);
    AssertIndexColor(buffer, 0, Color::kWhite);
  }
  WPI_SetNowImpl(nullptr);  // cleanup
}

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
