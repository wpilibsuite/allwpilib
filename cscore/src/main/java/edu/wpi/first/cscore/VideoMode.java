// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import edu.wpi.first.util.PixelFormat;
import java.util.Objects;

/** Video mode. */
@SuppressWarnings("MemberName")
public class VideoMode {
  /**
   * Create a new video mode.
   *
   * @param pixelFormat The pixel format enum as an integer.
   * @param width The image width in pixels.
   * @param height The image height in pixels.
   * @param fps The camera's frames per second.
   */
  public VideoMode(int pixelFormat, int width, int height, int fps) {
    this.pixelFormat = PixelFormat.getFromInt(pixelFormat);
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

  @Override
  public boolean equals(Object other) {
    if (this == other) {
      return true;
    }
    if (other == null) {
      return false;
    }

    return other instanceof VideoMode mode
        && pixelFormat == mode.pixelFormat
        && width == mode.width
        && height == mode.height
        && fps == mode.fps;
  }

  @Override
  public int hashCode() {
    return Objects.hash(pixelFormat, width, height, fps);
  }
}
