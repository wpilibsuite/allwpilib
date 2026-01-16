// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <stddef.h>  // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Raw Frame
 */
typedef struct WPI_RawFrame {  // NOLINT
  // image data
  uint8_t* data;
  // function to free image data (may be NULL)
  void (*freeFunc)(void* cbdata, void* data, size_t capacity);
  void* freeCbData;    // data passed to freeFunc
  size_t capacity;     // data buffer capacity, in bytes
  size_t size;         // actual size of data, in bytes
  int pixelFormat;     // WPI_PixelFormat
  int width;           // width of image, in pixels
  int height;          // height of image, in pixels
  int stride;          // size of each row of data, in bytes (may be 0)
  uint64_t timestamp;  // image capture timestamp
  int timestampSrc;    // WPI_TimestampSource
} WPI_RawFrame;

/**
 * Timestamp metadata. Timebase is the same as wpi::util::Now
 */
enum WPI_TimestampSource {
  WPI_TIMESRC_UNKNOWN = 0,    // unknown
  WPI_TIMESRC_FRAME_DEQUEUE,  // wpi::util::Now when the new frame was dequeued
                              // by CSCore. Does not account for camera exposure
                              // time or V4L latency.
  WPI_TIMESRC_V4L_EOF,  // End of Frame. Same as V4L2_BUF_FLAG_TSTAMP_SRC_EOF,
                        // translated into wpi::util::Now's timebase.
  WPI_TIMESRC_V4L_SOE,  // Start of Exposure. Same as
                        // V4L2_BUF_FLAG_TSTAMP_SRC_SOE, translated into
                        // wpi::util::Now's timebase.
};

// Returns nonzero if the frame data was allocated/reallocated
int WPI_AllocateRawFrameData(WPI_RawFrame* frame, size_t requestedSize);
void WPI_FreeRawFrameData(WPI_RawFrame* frame);
void WPI_SetRawFrameData(WPI_RawFrame* frame, void* data, size_t size,
                         size_t capacity, void* cbdata,
                         void (*freeFunc)(void* cbdata, void* data,
                                          size_t capacity));

#ifdef __cplusplus
}  // extern "C"
#endif
