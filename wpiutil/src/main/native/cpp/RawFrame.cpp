// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/RawFrame.h"

#include <wpi/MemAlloc.h>

#include <cstring>

extern "C" {
int WPI_AllocateRawFrameData(WPI_RawFrame* frame, size_t requestedSize) {
  if (frame->capacity >= requestedSize) {
    return 0;
  }
  WPI_FreeRawFrameData(frame);
  frame->data = static_cast<uint8_t*>(wpi::safe_malloc(requestedSize));
  frame->capacity = requestedSize;
  frame->size = 0;
  return 1;
}

void WPI_FreeRawFrameData(WPI_RawFrame* frame) {
  if (frame->data) {
    if (frame->freeFunc) {
      frame->freeFunc(frame->freeCbData, frame->data, frame->capacity);
    } else {
      std::free(frame->data);
    }
    frame->data = nullptr;
    frame->freeFunc = nullptr;
    frame->freeCbData = nullptr;
    frame->capacity = 0;
  }
}

void WPI_SetRawFrameData(WPI_RawFrame* frame, void* data, size_t size,
                         size_t capacity, void* cbdata,
                         void (*freeFunc)(void* cbdata, void* data,
                                          size_t capacity)) {
  WPI_FreeRawFrameData(frame);
  frame->data = static_cast<uint8_t*>(data);
  frame->freeFunc = freeFunc;
  frame->freeCbData = cbdata;
  frame->capacity = capacity;
  frame->size = size;
}

}  // extern "C"
