/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;

import java.util.Objects;

import edu.wpi.first.wpiutil.math.MathUtil;

/**
 * Represents colors with 8 bits of precision.
 */
@SuppressWarnings("MemberName")
public class Color8Bit {
  public final int red;
  public final int green;
  public final int blue;

  /**
   * Constructs a Color8Bit.
   *
   * @param red   Red value (0-255)
   * @param green Green value (0-255)
   * @param blue  Blue value (0-255)
   */
  public Color8Bit(int red, int green, int blue) {
    this.red = MathUtil.clamp(red, 0, 255);
    this.green = MathUtil.clamp(green, 0, 255);
    this.blue = MathUtil.clamp(blue, 0, 255);
  }

  /**
   * Constructs a Color8Bit from a Color.
   *
   * @param color The color
   */
  public Color8Bit(Color color) {
    this((int) (color.red * 255),
        (int) (color.green * 255),
        (int) (color.blue * 255));
  }

  @Override
  public boolean equals(Object other) {
    if (this == other) {
      return true;
    }
    if (other == null || getClass() != other.getClass()) {
      return false;
    }

    Color8Bit color8Bit = (Color8Bit) other;
    return red == color8Bit.red
        && green == color8Bit.green
        && blue == color8Bit.blue;
  }

  @Override
  public int hashCode() {
    return Objects.hash(red, green, blue);
  }
}
