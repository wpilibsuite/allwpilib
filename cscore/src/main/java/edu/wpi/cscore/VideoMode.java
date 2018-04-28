/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

/**
 * Video mode
 */
public class VideoMode {
  public enum PixelFormat {
    kUnknown(0), kMJPEG(1), kYUYV(2), kRGB565(3), kBGR(4), kGray(5);
    private int value;

    private PixelFormat(int value) {
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

  public VideoMode(int pixelFormat, int width, int height, int fps) {
    this.pixelFormat = getPixelFormatFromInt(pixelFormat);
    this.width = width;
    this.height = height;
    this.fps = fps;
  }

  public VideoMode(PixelFormat pixelFormat, int width, int height, int fps) {
    this.pixelFormat = pixelFormat;
    this.width = width;
    this.height = height;
    this.fps = fps;
  }

  /**
   * Pixel format
   */
  public PixelFormat pixelFormat;

  /**
   * Width in pixels
   */
  public int width;

  /**
   * Height in pixels
   */
  public int height;

  /**
   * Frames per second
   */
  public int fps;
}
