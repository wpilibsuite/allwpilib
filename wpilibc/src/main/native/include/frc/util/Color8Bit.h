// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>

#include <fmt/format.h>
#include <wpi/StringExtras.h>
#include <wpi/ct_string.h>

#include "Color.h"

namespace frc {

/**
 * Represents colors that can be used with Addressable LEDs.
 */
class Color8Bit {
 public:
  /**
   * Constructs a default color (black).
   */
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

  /**
   * Constructs a Color8Bit from a hex string.
   *
   * @param hexString a string of the format <tt>\#RRGGBB</tt>
   * @throws std::invalid_argument if the hex string is invalid.
   */
  explicit constexpr Color8Bit(std::string_view hexString) {
    if (hexString.length() != 7 || !hexString.starts_with("#") ||
        !wpi::isHexDigit(hexString[1]) || !wpi::isHexDigit(hexString[2]) ||
        !wpi::isHexDigit(hexString[3]) || !wpi::isHexDigit(hexString[4]) ||
        !wpi::isHexDigit(hexString[5]) || !wpi::isHexDigit(hexString[6])) {
      throw std::invalid_argument(
          fmt::format("Invalid hex string for Color \"{}\"", hexString));
    }

    red = wpi::hexDigitValue(hexString[1]) * 16 +
          wpi::hexDigitValue(hexString[2]);
    green = wpi::hexDigitValue(hexString[3]) * 16 +
            wpi::hexDigitValue(hexString[4]);
    blue = wpi::hexDigitValue(hexString[5]) * 16 +
           wpi::hexDigitValue(hexString[6]);
  }

  constexpr bool operator==(const Color8Bit&) const = default;

  constexpr operator Color() const {  // NOLINT
    return Color(red / 255.0, green / 255.0, blue / 255.0);
  }

  /**
   * Create a Color8Bit from a hex string.
   *
   * @param hexString a string of the format <tt>\#RRGGBB</tt>
   * @return Color8Bit object from hex string.
   * @throws std::invalid_argument if the hex string is invalid.
   */
  static constexpr Color8Bit FromHexString(std::string_view hexString) {
    if (hexString.length() != 7 || !hexString.starts_with("#") ||
        !wpi::isHexDigit(hexString[1]) || !wpi::isHexDigit(hexString[2]) ||
        !wpi::isHexDigit(hexString[3]) || !wpi::isHexDigit(hexString[4]) ||
        !wpi::isHexDigit(hexString[5]) || !wpi::isHexDigit(hexString[6])) {
      throw std::invalid_argument(
          fmt::format("Invalid hex string for Color \"{}\"", hexString));
    }

    int r = wpi::hexDigitValue(hexString[0]) * 16 +
            wpi::hexDigitValue(hexString[1]);
    int g = wpi::hexDigitValue(hexString[2]) * 16 +
            wpi::hexDigitValue(hexString[3]);
    int b = wpi::hexDigitValue(hexString[4]) * 16 +
            wpi::hexDigitValue(hexString[5]);
    return Color8Bit{r, g, b};
  }

  /**
   * Return this color represented as a hex string.
   *
   * @return a string of the format <tt>\#RRGGBB</tt>
   */
  constexpr auto HexString() const {
    return wpi::ct_string<char, std::char_traits<char>, 7>{
        {'#', wpi::hexdigit(red / 16), wpi::hexdigit(red % 16),
         wpi::hexdigit(green / 16), wpi::hexdigit(green % 16),
         wpi::hexdigit(blue / 16), wpi::hexdigit(blue % 16)}};
  }

  /// Red component (0-255).
  int red = 0;

  /// Green component (0-255).
  int green = 0;

  /// Blue component (0-255).
  int blue = 0;
};

}  // namespace frc
