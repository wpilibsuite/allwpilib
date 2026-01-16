// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/cs/cscore_c.h"
#include "wpi/util/PixelFormat.hpp"

namespace wpi::cs {

/**
 * Video mode
 */
struct VideoMode {
  VideoMode() = default;
  VideoMode(wpi::util::PixelFormat pixelFormat_, int width_, int height_,
            int fps_)
      : pixelFormat{pixelFormat_}, width{width_}, height{height_}, fps{fps_} {}
  VideoMode(const CS_VideoMode& mode)  // NOLINT
      : pixelFormat{static_cast<wpi::util::PixelFormat>(mode.pixelFormat)},
        width{mode.width},
        height{mode.height},
        fps{mode.fps} {}

  operator CS_VideoMode() const {  // NOLINT
    CS_VideoMode mode;
    mode.pixelFormat = static_cast<int>(pixelFormat);
    mode.width = width;
    mode.height = height;
    mode.fps = fps;
    return mode;
  }

  explicit operator bool() const {
    return pixelFormat == wpi::util::PixelFormat::kUnknown;
  }

  bool operator==(const VideoMode& other) const {
    return pixelFormat == other.pixelFormat && width == other.width &&
           height == other.height && fps == other.fps;
  }

  bool CompareWithoutFps(const VideoMode& other) const {
    return pixelFormat == other.pixelFormat && width == other.width &&
           height == other.height;
  }

  wpi::util::PixelFormat pixelFormat = wpi::util::PixelFormat::kUnknown;
  int width = 0;
  int height = 0;
  int fps = 0;
};

}  // namespace wpi::cs
