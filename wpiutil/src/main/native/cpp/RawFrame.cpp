// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/RawFrame.h"

#include <wpi/MemAlloc.h>

extern "C" {
void WPI_AllocateRawFrameData(WPI_RawFrame* frame, int requestedSize) {
  if (frame->dataLength >= requestedSize) {
    return;
  }
  if (frame->data) {
    frame->data =
        static_cast<char*>(wpi::safe_realloc(frame->data, requestedSize));
  } else {
    frame->data = static_cast<char*>(wpi::safe_malloc(requestedSize));
  }
  frame->dataLength = requestedSize;
}

void WPI_FreeRawFrameData(WPI_RawFrame* frame) {
  if (frame->data) {
    std::free(frame->data);
    frame->data = nullptr;
    frame->dataLength = 0;
  }
}
}  // extern "C"
