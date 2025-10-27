// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj.util.Color8Bit;

/** Generic interface for writing data to an LED buffer. */
@FunctionalInterface
public interface LEDWriter {
  /**
   * Sets the RGB value for an LED at a specific index on a LED buffer.
   *
   * @param index the index of the LED to write to
   * @param r the value of the red channel, in [0, 255]
   * @param g the value of the green channel, in [0, 255]
   * @param b the value of the blue channel, in [0, 255]
   */
  void setRGB(int index, int r, int g, int b);

  /**
   * Sets a specific led in the buffer.
   *
   * @param index the index to write
   * @param h the h value [0-180)
   * @param s the s value [0-255]
   * @param v the v value [0-255]
   */
  default void setHSV(int index, int h, int s, int v) {
    if (s == 0) {
      setRGB(index, v, v, v);
      return;
    }

    int packedRGB = Color.hsvToRgb(h, s, v);

    setRGB(
        index,
        Color.unpackRGB(packedRGB, Color.RGBChannel.kRed),
        Color.unpackRGB(packedRGB, Color.RGBChannel.kGreen),
        Color.unpackRGB(packedRGB, Color.RGBChannel.kBlue));
  }

  /**
   * Sets the RGB value for an LED at a specific index on a LED buffer.
   *
   * @param index the index of the LED to write to
   * @param color the color to set
   */
  default void setLED(int index, Color color) {
    setRGB(index, (int) (color.red * 255), (int) (color.green * 255), (int) (color.blue * 255));
  }

  /**
   * Sets the RGB value for an LED at a specific index on a LED buffer.
   *
   * @param index the index of the LED to write to
   * @param color the color to set
   */
  default void setLED(int index, Color8Bit color) {
    setRGB(index, color.red, color.green, color.blue);
  }
}
