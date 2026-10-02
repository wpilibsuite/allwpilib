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
  UNKNOWN = WPI_PIXFMT_UNKNOWN,
  MJPEG = WPI_PIXFMT_MJPEG,
  YUYV = WPI_PIXFMT_YUYV,
  RGB565 = WPI_PIXFMT_RGB565,
  BGR = WPI_PIXFMT_BGR,
  GRAY = WPI_PIXFMT_GRAY,
  Y16 = WPI_PIXFMT_Y16,
  UYVY = WPI_PIXFMT_UYVY,
  BGRA = WPI_PIXFMT_BGRA,
};

}  // namespace wpi::util
