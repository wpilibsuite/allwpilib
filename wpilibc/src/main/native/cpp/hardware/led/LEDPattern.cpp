// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/LEDPattern.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <unordered_map>
#include <utility>
#include <vector>

#include <wpi/MathExtras.h>
#include <wpi/timestamp.h>

#include "frc/MathUtil.h"

using namespace frc;

LEDPattern::LEDPattern(std::function<void(frc::LEDPattern::LEDReader,
                                          std::function<void(int, frc::Color)>)>
                           impl)
    : m_impl(std::move(impl)) {}

void LEDPattern::ApplyTo(LEDPattern::LEDReader reader,
                         std::function<void(int, frc::Color)> writer) const {
  m_impl(reader, writer);
}

void LEDPattern::ApplyTo(std::span<AddressableLED::LEDData> data,
                         std::function<void(int, frc::Color)> writer) const {
  ApplyTo(LEDPattern::LEDReader{[=](size_t i) { return data[i]; }, data.size()},
          writer);
}

void LEDPattern::ApplyTo(std::span<AddressableLED::LEDData> data) const {
  ApplyTo(data, [&](int index, Color color) { data[index].SetLED(color); });
}

LEDPattern LEDPattern::MapIndex(
    std::function<size_t(size_t, size_t)> indexMapper) {
  return LEDPattern{[self = *this, indexMapper](auto data, auto writer) {
    size_t bufLen = data.size();
    self.ApplyTo(
        LEDPattern::LEDReader{
            [=](auto i) { return data[indexMapper(bufLen, i)]; }, bufLen},
        [&](int i, Color color) { writer(indexMapper(bufLen, i), color); });
  }};
}

LEDPattern LEDPattern::Reversed() {
  return MapIndex([](size_t bufLen, size_t i) { return bufLen - 1 - i; });
}

LEDPattern LEDPattern::OffsetBy(int offset) {
  return MapIndex([offset](size_t bufLen, size_t i) {
    return frc::FloorMod(static_cast<int>(i) + offset,
                         static_cast<int>(bufLen));
  });
}

LEDPattern LEDPattern::ScrollAtRelativeSpeed(units::hertz_t velocity) {
  // velocity is in terms of LED lengths per second (1_hz = full cycle per
  // second, 0.5_hz = half cycle per second, 2_hz = two cycles per second)
  // Invert and multiply by 1,000,000 to get microseconds
  double periodMicros = 1e6 / velocity.value();

  return MapIndex([=](size_t bufLen, size_t i) {
    auto now = wpi::Now();

    // index should move by (bufLen) / (period)
    double t =
        (now % static_cast<int64_t>(std::floor(periodMicros))) / periodMicros;
    int offset = static_cast<int>(std::floor(t * bufLen));

    return frc::FloorMod(static_cast<int>(i) + offset,
                         static_cast<int>(bufLen));
  });
}

LEDPattern LEDPattern::ScrollAtAbsoluteSpeed(
    units::meters_per_second_t velocity, units::meter_t ledSpacing) {
  // Velocity is in terms of meters per second
  // Multiply by 1,000,000 to use microseconds instead of seconds
  auto microsPerLed =
      static_cast<int64_t>(std::floor((ledSpacing / velocity).value() * 1e6));

  return MapIndex([=](size_t bufLen, size_t i) {
    auto now = wpi::Now();

    // every step in time that's a multiple of microsPerLED will increment
    // the offset by 1
    // cast unsigned int64 `now` to a signed int64 so we can get negative
    // offset values for negative velocities
    auto offset = static_cast<int64_t>(now) / microsPerLed;

    return frc::FloorMod(static_cast<int>(i) + offset,
                         static_cast<int>(bufLen));
  });
}

LEDPattern LEDPattern::Blink(units::second_t onTime, units::second_t offTime) {
  auto totalMicros = units::microsecond_t{onTime + offTime}.to<uint64_t>();
  auto onMicros = units::microsecond_t{onTime}.to<uint64_t>();

  return LEDPattern{[=, self = *this](auto data, auto writer) {
    if (wpi::Now() % totalMicros < onMicros) {
      self.ApplyTo(data, writer);
    } else {
      LEDPattern::Off().ApplyTo(data, writer);
    }
  }};
}

LEDPattern LEDPattern::Blink(units::second_t onTime) {
  return LEDPattern::Blink(onTime, onTime);
}

LEDPattern LEDPattern::SynchronizedBlink(std::function<bool()> signal) {
  return LEDPattern{[=, self = *this](auto data, auto writer) {
    if (signal()) {
      self.ApplyTo(data, writer);
    } else {
      LEDPattern::Off().ApplyTo(data, writer);
    }
  }};
}

LEDPattern LEDPattern::Breathe(units::second_t period) {
  auto periodMicros = units::microsecond_t{period};

  return LEDPattern{[periodMicros, self = *this](auto data, auto writer) {
    self.ApplyTo(data, [&writer, periodMicros](int i, Color color) {
      double t = (wpi::Now() % periodMicros.to<uint64_t>()) /
                 periodMicros.to<double>();
      double phase = t * 2 * std::numbers::pi;

      // Apply the cosine function and shift its output from [-1, 1] to [0, 1]
      // Use cosine so the period starts at 100% brightness
      double dim = (std::cos(phase) + 1) / 2.0;

      writer(i, Color{color.red * dim, color.green * dim, color.blue * dim});
    });
  }};
}

LEDPattern LEDPattern::OverlayOn(const LEDPattern& base) {
  return LEDPattern{[self = *this, base](auto data, auto writer) {
    // write the base pattern down first...
    base.ApplyTo(data, writer);

    // ... then, overwrite with illuminated LEDs from the overlay
    self.ApplyTo(data, [&](int i, Color color) {
      if (color.red > 0 || color.green > 0 || color.blue > 0) {
        writer(i, color);
      }
    });
  }};
}

LEDPattern LEDPattern::Blend(const LEDPattern& other) {
  return LEDPattern{[self = *this, other](auto data, auto writer) {
    // Apply the current pattern down as normal...
    self.ApplyTo(data, writer);

    other.ApplyTo(data, [&](int i, Color color) {
      // ... then read the result and average it with the output from the other
      // pattern
      writer(i, Color{(data[i].r / 255.0 + color.red) / 2,
                      (data[i].g / 255.0 + color.green) / 2,
                      (data[i].b / 255.0 + color.blue) / 2});
    });
  }};
}

LEDPattern LEDPattern::Mask(const LEDPattern& mask) {
  return LEDPattern{[self = *this, mask](auto data, auto writer) {
    // Apply the current pattern down as normal...
    self.ApplyTo(data, writer);

    mask.ApplyTo(data, [&](int i, Color color) {
      auto currentColor = data[i];
      // ... then perform a bitwise AND operation on each channel to apply the
      // mask
      writer(i, Color{currentColor.r & static_cast<uint8_t>(255 * color.red),
                      currentColor.g & static_cast<uint8_t>(255 * color.green),
                      currentColor.b & static_cast<uint8_t>(255 * color.blue)});
    });
  }};
}

LEDPattern LEDPattern::AtBrightness(double relativeBrightness) {
  return LEDPattern{[relativeBrightness, self = *this](auto data, auto writer) {
    self.ApplyTo(data, [&](int i, Color color) {
      writer(i, Color{color.red * relativeBrightness,
                      color.green * relativeBrightness,
                      color.blue * relativeBrightness});
    });
  }};
}

// Static constants and functions

LEDPattern LEDPattern::Off() {
  return LEDPattern::Solid(Color::kBlack);
}

LEDPattern LEDPattern::Solid(const Color color) {
  return LEDPattern{[=](auto data, auto writer) {
    auto bufLen = data.size();
    for (size_t i = 0; i < bufLen; i++) {
      writer(i, color);
    }
  }};
}

LEDPattern LEDPattern::ProgressMaskLayer(
    std::function<double()> progressFunction) {
  return LEDPattern{[=](auto data, auto writer) {
    double progress = std::clamp(progressFunction(), 0.0, 1.0);
    auto bufLen = data.size();
    size_t max = bufLen * progress;

    for (size_t led = 0; led < max; led++) {
      writer(led, Color::kWhite);
    }
    for (size_t led = max; led < bufLen; led++) {
      writer(led, Color::kBlack);
    }
  }};
}

LEDPattern LEDPattern::Steps(std::span<const std::pair<double, Color>> steps) {
  if (steps.size() == 0) {
    // no colors specified
    return LEDPattern::Off();
  }
  if (steps.size() == 1 && steps[0].first == 0) {
    // only one color specified, just show a static color
    return LEDPattern::Solid(steps[0].second);
  }

  return LEDPattern{[steps = std::vector(steps.begin(), steps.end())](
                        auto data, auto writer) {
    auto bufLen = data.size();

    // precompute relevant positions for this buffer so we don't need to do a
    // check on every single LED index
    std::unordered_map<int, Color> stopPositions;

    for (auto step : steps) {
      stopPositions[std::floor(step.first * bufLen)] = step.second;
    }
    auto currentColor = Color::kBlack;
    for (size_t led = 0; led < bufLen; led++) {
      if (stopPositions.contains(led)) {
        currentColor = stopPositions[led];
      }
      writer(led, currentColor);
    }
  }};
}

LEDPattern LEDPattern::Steps(
    std::initializer_list<std::pair<double, Color>> steps) {
  return Steps(std::span{steps.begin(), steps.end()});
}

LEDPattern LEDPattern::Gradient(GradientType type,
                                std::span<const Color> colors) {
  if (colors.size() == 0) {
    // no colors specified
    return LEDPattern::Off();
  }
  if (colors.size() == 1) {
    // only one color specified, just show a static color
    return LEDPattern::Solid(colors[0]);
  }

  return LEDPattern{[type, colors = std::vector(colors.begin(), colors.end())](
                        auto data, auto writer) {
    size_t numSegments = colors.size();
    auto bufLen = data.size();
    int ledsPerSegment = 0;
    switch (type) {
      case kContinuous:
        ledsPerSegment = bufLen / numSegments;
        break;
      case kDiscontinuous:
        ledsPerSegment = (bufLen - 1) / (numSegments - 1);
        break;
    }

    for (size_t led = 0; led < bufLen; led++) {
      int colorIndex = (led / ledsPerSegment) % numSegments;
      int nextColorIndex = (colorIndex + 1) % numSegments;
      double t = std::fmod(led / static_cast<double>(ledsPerSegment), 1.0);

      auto color = colors[colorIndex];
      auto nextColor = colors[nextColorIndex];

      Color gradientColor{wpi::Lerp(color.red, nextColor.red, t),
                          wpi::Lerp(color.green, nextColor.green, t),
                          wpi::Lerp(color.blue, nextColor.blue, t)};
      writer(led, gradientColor);
    }
  }};
}

LEDPattern LEDPattern::Gradient(GradientType type,
                                std::initializer_list<Color> colors) {
  return Gradient(type, std::span{colors.begin(), colors.end()});
}

LEDPattern LEDPattern::Rainbow(int saturation, int value) {
  return LEDPattern{[=](auto data, auto writer) {
    auto bufLen = data.size();
    for (size_t led = 0; led < bufLen; led++) {
      int hue = ((led * 180) / bufLen) % 180;
      writer(led, Color::FromHSV(hue, saturation, value));
    }
  }};
}
