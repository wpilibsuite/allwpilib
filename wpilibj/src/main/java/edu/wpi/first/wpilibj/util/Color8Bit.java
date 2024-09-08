// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.util;

import edu.wpi.first.math.MathUtil;
import java.util.Objects;

/** Represents colors with 8 bits of precision. */
@SuppressWarnings("MemberName")
public class Color8Bit {
  /** Red component (0-255). */
  public final int red;

  /** Green component (0-255). */
  public final int green;

  /** Blue component (0-255). */
  public final int blue;

  /** Constructs a default color (black). */
  public Color8Bit() {
    red = 0;
    green = 0;
    blue = 0;
  }

  /**
   * Constructs a Color8Bit.
   *
   * @param red Red value (0-255)
   * @param green Green value (0-255)
   * @param blue Blue value (0-255)
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
    this((int) (color.red * 255), (int) (color.green * 255), (int) (color.blue * 255));
  }

  /**
   * Constructs a Color8Bit from a hex string.
   *
   * @param hexString a string of the format <code>#RRGGBB</code>
   * @throws IllegalArgumentException if the hex string is invalid.
   */
  public Color8Bit(String hexString) {
    if (hexString.length() != 7 || !hexString.startsWith("#")) {
      throw new IllegalArgumentException("Invalid hex string \"" + hexString + "\"");
    }

    this.red = Integer.valueOf(hexString.substring(1, 3), 16);
    this.green = Integer.valueOf(hexString.substring(3, 5), 16);
    this.blue = Integer.valueOf(hexString.substring(5, 7), 16);
  }

  @Override
  public boolean equals(Object other) {
    if (this == other) {
      return true;
    }
    if (other == null) {
      return false;
    }

    return other instanceof Color8Bit color8Bit
        && red == color8Bit.red
        && green == color8Bit.green
        && blue == color8Bit.blue;
  }

  @Override
  public int hashCode() {
    return Objects.hash(red, green, blue);
  }

  @Override
  public String toString() {
    return toHexString();
  }

  /**
   * Return this color represented as a hex string.
   *
   * @return a string of the format <code>#RRGGBB</code>
   */
  public String toHexString() {
    return String.format("#%02X%02X%02X", red, green, blue);
  }
}
