// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_RAWFRAME_H_
#define WPIUTIL_WPI_RAWFRAME_H_

/**
 * Raw Frame
 */
typedef struct WPI_RawFrame {  // NOLINT
  char* data;
  int dataLength;
  int pixelFormat;
  int width;
  int height;
  int totalData;
} WPI_RawFrame;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Pixel formats
 */
enum WPI_PixelFormat {
  WPI_PIXFMT_UNKNOWN = 0,
  WPI_PIXFMT_MJPEG,
  WPI_PIXFMT_YUYV,
  WPI_PIXFMT_RGB565,
  WPI_PIXFMT_BGR,
  WPI_PIXFMT_GRAY,
  WPI_PIXFMT_Y16,
  WPI_PIXFMT_UYVY
};

void WPI_AllocateRawFrameData(WPI_RawFrame* frame, int requestedSize);
void WPI_FreeRawFrameData(WPI_RawFrame* frame);

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef __cplusplus
namespace wpi {
struct RawFrame : public WPI_RawFrame {
  RawFrame() {
    data = nullptr;
    dataLength = 0;
    pixelFormat = WPI_PIXFMT_UNKNOWN;
    width = 0;
    height = 0;
    totalData = 0;
  }

  ~RawFrame() { WPI_FreeRawFrameData(this); }
};
}  // namespace wpi

#endif

#endif  // WPIUTIL_WPI_RAWFRAME_H_
