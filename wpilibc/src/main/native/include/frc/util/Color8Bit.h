// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>

#include "Color.h"

namespace frc {

/**
 * Represents colors that can be used with Addressable LEDs.
 */
class Color8Bit {
 public:
  constexpr Color8Bit() = default;

  /**
   * Constructs a Color8Bit.
   *
   * @param red Red value (0-255)
   * @param green Green value (0-255)
   * @param blue Blue value (0-255)
   */
  constexpr Color8Bit(int r, int g, int b)
      : red(std::clamp(r, 0, 255)),
        green(std::clamp(g, 0, 255)),
        blue(std::clamp(b, 0, 255)) {}

  /**
   * Constructs a Color8Bit from a Color.
   *
   * @param color The color
   */
  constexpr Color8Bit(const Color& color)  // NOLINT
      : red(color.red * 255),
        green(color.green * 255),
        blue(color.blue * 255) {}

  constexpr operator Color() const {  // NOLINT
    return Color(red / 255.0, green / 255.0, blue / 255.0);
  }

  int red = 0;
  int green = 0;
  int blue = 0;
};

inline bool operator==(const Color8Bit& c1, const Color8Bit& c2) {
  return c1.red == c2.red && c1.green == c2.green && c1.blue == c2.blue;
}

}  // namespace frc
