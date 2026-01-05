// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/cs/cscore_c.h"
#include "wpi/util/RawFrame.h"

namespace wpi::cs {

/**
 * Video mode
 */
struct VideoMode : public CS_VideoMode {
  enum PixelFormat {
    kUnknown = WPI_PIXFMT_UNKNOWN,
    kMJPEG = WPI_PIXFMT_MJPEG,
    kYUYV = WPI_PIXFMT_YUYV,
    kRGB565 = WPI_PIXFMT_RGB565,
    kBGR = WPI_PIXFMT_BGR,
    kGray = WPI_PIXFMT_GRAY,
    kY16 = WPI_PIXFMT_Y16,
    kUYVY = WPI_PIXFMT_UYVY,
    kBGRA = WPI_PIXFMT_BGRA,
  };
  VideoMode() {
    pixelFormat = 0;
    width = 0;
    height = 0;
    fps = 0;
  }
  VideoMode(PixelFormat pixelFormat_, int width_, int height_, int fps_) {
    pixelFormat = pixelFormat_;
    width = width_;
    height = height_;
    fps = fps_;
  }
  explicit operator bool() const { return pixelFormat == kUnknown; }

  bool operator==(const VideoMode& other) const {
    return pixelFormat == other.pixelFormat && width == other.width &&
           height == other.height && fps == other.fps;
  }

  bool CompareWithoutFps(const VideoMode& other) const {
    return pixelFormat == other.pixelFormat && width == other.width &&
           height == other.height;
  }
};

}  // namespace wpi::cs
