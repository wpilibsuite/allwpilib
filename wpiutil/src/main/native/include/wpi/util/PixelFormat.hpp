// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/util/PixelFormat.h"

namespace wpi::util {

/**
 * Pixel formats
 */
enum class PixelFormat {
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

}  // namespace wpi::util
