// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/** Video mode. */
@SuppressWarnings("MemberName")
public class VideoMode {
  public enum PixelFormat {
    kUnknown(0),
    kMJPEG(1),
    kYUYV(2),
    kRGB565(3),
    kBGR(4),
    kGray(5),
    kY16(6),
    kUYVY(7);

    private final int value;

    PixelFormat(int value) {
      this.value = value;
    }

    public int getValue() {
      return value;
    }
  }

  private static final PixelFormat[] m_pixelFormatValues = PixelFormat.values();

  public static PixelFormat getPixelFormatFromInt(int pixelFormat) {
    return m_pixelFormatValues[pixelFormat];
  }

  /**
   * Create a new video mode.
   *
   * @param pixelFormat The pixel format enum as an integer.
   * @param width The image width in pixels.
   * @param height The image height in pixels.
   * @param fps The camera's frames per second.
   */
  public VideoMode(int pixelFormat, int width, int height, int fps) {
    this.pixelFormat = getPixelFormatFromInt(pixelFormat);
    this.width = width;
    this.height = height;
    this.fps = fps;
  }

  /**
   * Create a new video mode.
   *
   * @param pixelFormat The pixel format.
   * @param width The image width in pixels.
   * @param height The image height in pixels.
   * @param fps The camera's frames per second.
   */
  public VideoMode(PixelFormat pixelFormat, int width, int height, int fps) {
    this.pixelFormat = pixelFormat;
    this.width = width;
    this.height = height;
    this.fps = fps;
  }

  /** Pixel format. */
  public PixelFormat pixelFormat;

  /** Width in pixels. */
  public int width;

  /** Height in pixels. */
  public int height;

  /** Frames per second. */
  public int fps;
}
