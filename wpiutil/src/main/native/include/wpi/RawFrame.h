// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_RAWFRAME_H_
#define WPIUTIL_WPI_RAWFRAME_H_

#include <stdint.h>

#ifdef __cplusplus
#include <concepts>
#include <cstddef>
#else

#include <stddef.h>  // NOLINT

#endif

#ifdef WPI_RAWFRAME_JNI
#include "jni_util.h"
#endif

// NOLINT

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
  void* freeCbData;  // data passed to freeFunc
  size_t capacity;   // data buffer capacity, in bytes
  size_t size;       // actual size of data, in bytes
  int pixelFormat;   // WPI_PixelFormat
  int width;         // width of image, in pixels
  int height;        // height of image, in pixels
  int stride;        // size of each row of data, in bytes (may be 0)
} WPI_RawFrame;

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

#ifdef __cplusplus
namespace wpi {
struct RawFrame : public WPI_RawFrame {
  RawFrame() {
    data = nullptr;
    freeFunc = nullptr;
    freeCbData = nullptr;
    capacity = 0;
    size = 0;
    pixelFormat = WPI_PIXFMT_UNKNOWN;
    width = 0;
    height = 0;
  }
  RawFrame(const RawFrame&) = delete;
  RawFrame& operator=(const RawFrame&) = delete;
  RawFrame(RawFrame&& rhs) noexcept : WPI_RawFrame{rhs} {
    rhs.data = nullptr;
    rhs.freeFunc = nullptr;
    rhs.freeCbData = nullptr;
    rhs.capacity = 0;
    rhs.size = 0;
  }
  RawFrame& operator=(RawFrame&& rhs) noexcept {
    *static_cast<WPI_RawFrame*>(this) = rhs;
    rhs.data = nullptr;
    rhs.freeFunc = nullptr;
    rhs.freeCbData = nullptr;
    rhs.capacity = 0;
    rhs.size = 0;
    return *this;
  }

  void SetData(void* data, size_t size, size_t capacity, void* cbdata,
               void (*freeFunc)(void* cbdata, void* data, size_t capacity)) {
    WPI_SetRawFrameData(this, data, size, capacity, cbdata, freeFunc);
  }

  // returns true if the frame data was allocated/reallocated
  bool Reserve(size_t size) {
    return WPI_AllocateRawFrameData(this, size) != 0;
  }

  ~RawFrame() { WPI_FreeRawFrameData(this); }
};

#ifdef WPI_RAWFRAME_JNI
template <std::same_as<wpi::RawFrame> T>
void SetFrameData(JNIEnv* env, jclass rawFrameCls, jobject jframe,
                  const T& frame, bool newData) {
  if (newData) {
    static jmethodID setData = env->GetMethodID(rawFrameCls, "setDataJNI",
                                                "(Ljava/nio/ByteBuffer;IIII)V");
    env->CallVoidMethod(
        jframe, setData, env->NewDirectByteBuffer(frame.data, frame.size),
        static_cast<jint>(frame.width), static_cast<jint>(frame.height),
        static_cast<jint>(frame.stride), static_cast<jint>(frame.pixelFormat));
  } else {
    static jmethodID setInfo =
        env->GetMethodID(rawFrameCls, "setInfoJNI", "(IIII)V");
    env->CallVoidMethod(jframe, setInfo, static_cast<jint>(frame.width),
                        static_cast<jint>(frame.height),
                        static_cast<jint>(frame.stride),
                        static_cast<jint>(frame.pixelFormat));
  }
}
#endif

}  // namespace wpi
#endif

#endif  // WPIUTIL_WPI_RAWFRAME_H_
