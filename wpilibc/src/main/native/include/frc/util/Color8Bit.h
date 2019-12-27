/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Color.h"

namespace frc {

class Color;

/**
 * Represents colors that can be used with Addressable LEDs.
 */
class Color8Bit {
 public:
  /**
   * Constructs a Color8Bit.
   *
   * @param red Red value (0-255)
   * @param green Green value (0-255)
   * @param blue Blue value (0-255)
   */
  constexpr Color8Bit(int r, int g, int b) : red(r), green(g), blue(b) {}

  /**
   * Constructs a Color8Bit from a Color.
   *
   * @param color The color
   */
  constexpr Color8Bit(Color color);

  int red;
  int green;
  int blue;
};

bool operator==(const Color8Bit& c1, const Color8Bit& c2) {
  return c1.red == c2.red &&
      c1.green == c2.green &&
      c1.blue == c2.blue;
}

}  // namespace frc
