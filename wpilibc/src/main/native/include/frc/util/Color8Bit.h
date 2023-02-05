// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>

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
   * @param r Red value (0-255)
   * @param g Green value (0-255)
   * @param b Blue value (0-255)
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

  /**
   * Create a Color8Bit from a hex string. Throws an exception if the Hex String
   * is invalid.
   *
   * @param hexString a string of the format <tt>\#RRGGBB</tt>
   * @return Color8Bit object from hex string.
   */
  static constexpr Color8Bit FromHexString(std::string hexString) {
    if (hexString.length() != 7 || !hexString.starts_with("#")) {
      throw std::invalid_argument("Invalid Hex String for Color");
    }

    int r, g, b;
    std::sscanf(hexString.c_str(), "#%02x%02x%02x", &r, &g, &b);
    return Color8Bit(r, g, b);
  }

  constexpr bool operator==(const Color8Bit&) const = default;

  /**
   * Return this color represented as a hex string.
   *
   * @return a string of the format <tt>\#RRGGBB</tt>
   */
  std::string HexString() const;

  int red = 0;
  int green = 0;
  int blue = 0;
};

}  // namespace frc
