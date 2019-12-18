/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;

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
    this.red = red;
    this.green = green;
    this.blue = blue;
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
}
