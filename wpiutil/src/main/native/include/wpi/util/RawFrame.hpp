// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/util/PixelFormat.h"
#include "wpi/util/RawFrame.h"

#ifdef WPI_RAWFRAME_JNI
#include <concepts>
#endif
#include <cstddef>

#ifdef WPI_RAWFRAME_JNI
#include "wpi/util/jni_util.hpp"
#endif

namespace wpi::util {
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
    timestamp = 0;
    timestampSrc = WPI_TIMESRC_UNKNOWN;
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
template <std::same_as<wpi::util::RawFrame> T>
void SetFrameData(JNIEnv* env, jclass rawFrameCls, jobject jframe,
                  const T& frame, bool newData) {
  if (newData) {
    static jmethodID setData = env->GetMethodID(
        rawFrameCls, "setDataJNI", "(Ljava/nio/ByteBuffer;IIIIJI)V");
    env->CallVoidMethod(
        jframe, setData, env->NewDirectByteBuffer(frame.data, frame.size),
        static_cast<jint>(frame.width), static_cast<jint>(frame.height),
        static_cast<jint>(frame.stride), static_cast<jint>(frame.pixelFormat),
        static_cast<jlong>(frame.timestamp),
        static_cast<jint>(frame.timestampSrc));
  } else {
    static jmethodID setInfo =
        env->GetMethodID(rawFrameCls, "setInfoJNI", "(IIIIJI)V");
    env->CallVoidMethod(jframe, setInfo, static_cast<jint>(frame.width),
                        static_cast<jint>(frame.height),
                        static_cast<jint>(frame.stride),
                        static_cast<jint>(frame.pixelFormat),
                        static_cast<jlong>(frame.timestamp),
                        static_cast<jint>(frame.timestampSrc));
  }
}
#endif

}  // namespace wpi::util
