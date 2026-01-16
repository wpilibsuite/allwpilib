// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

/**
 * Pixel formats
 */
enum WPI_PixelFormat {
  WPI_PIXFMT_UNKNOWN = 0,  // unknown
  WPI_PIXFMT_MJPEG,        // Motion-JPEG (compressed image data)
  WPI_PIXFMT_YUYV,         // YUV 4:2:2, 16 bpp
  WPI_PIXFMT_RGB565,       // RGB 5-6-5, 16 bpp
  WPI_PIXFMT_BGR,          // BGR 8-8-8, 24 bpp
  WPI_PIXFMT_GRAY,         // Grayscale, 8 bpp
  WPI_PIXFMT_Y16,          // Grayscale, 16 bpp
  WPI_PIXFMT_UYVY,         // YUV 4:2:2, 16 bpp
  WPI_PIXFMT_BGRA,         // BGRA 8-8-8-8-, 32 bpp
};
