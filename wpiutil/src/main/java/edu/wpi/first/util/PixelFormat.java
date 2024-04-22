// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

/** Image pixel format. */
public enum PixelFormat {
  /** Unknown format. */
  kUnknown(0),
  /** Motion-JPEG (compressed image data). */
  kMJPEG(1),
  /** YUY 4:2:2, 16 bpp. */
  kYUYV(2),
  /** RGB 5-6-5, 16 bpp. */
  kRGB565(3),
  /** BGR 8-8-8, 24 bpp. */
  kBGR(4),
  /** Grayscale, 8 bpp. */
  kGray(5),
  /** Grayscale, 16 bpp. */
  kY16(6),
  /** YUV 4:2:2, 16 bpp. */
  kUYVY(7),
  /** BGRA 8-8-8-8. 32 bpp. */
  kBGRA(8);

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
