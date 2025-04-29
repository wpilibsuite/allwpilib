// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

/** Image pixel format. */
public enum PixelFormat {
  /** Unknown format. */
  UNKNOWN(0),
  /** Motion-JPEG (compressed image data). */
  MJPEG(1),
  /** YUY 4:2:2, 16 bpp. */
  YUYV(2),
  /** RGB 5-6-5, 16 bpp. */
  RGB565(3),
  /** BGR 8-8-8, 24 bpp. */
  BGR(4),
  /** Grayscale, 8 bpp. */
  GRAY(5),
  /** Grayscale, 16 bpp. */
  Y16(6),
  /** YUV 4:2:2, 16 bpp. */
  UYVY(7),
  /** BGRA 8-8-8-8. 32 bpp. */
  BGRA(8);

  private final int value;

  PixelFormat(int value) {
    this.value = value;
  }

  /**
   * Gets the integer value of the pixel format.
   *
   * @return Integer value
   */
  public int getValue() {
    return value;
  }

  private static final PixelFormat[] s_values = values();

  /**
   * Gets a PixelFormat enum value from its integer value.
   *
   * @param pixelFormat integer value
   * @return Enum value
   */
  public static PixelFormat getFromInt(int pixelFormat) {
    return s_values[pixelFormat];
  }
}
