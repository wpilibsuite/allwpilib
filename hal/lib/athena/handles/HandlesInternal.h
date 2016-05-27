/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Handles.h"

namespace hal {
static inline int16_t getHandleIndex(HalHandle handle) {
  return (int16_t)(handle & 0xffff);
}
static inline HalHandleEnum getHandleType(HalHandle handle) {
  return (HalHandleEnum)((handle >> 24) & 0xff);
}
static inline bool isHandleType(HalHandle handle, HalHandleEnum handleType) {
  return handleType == getHandleType(handleType);
}
static inline int16_t getHandleTypedIndex(HalHandle handle,
                                          HalHandleEnum enumType) {
  if (!isHandleType(handle, enumType)) return HAL_HANDLE_INVALID_TYPE;
  return getHandleIndex(handle);
}

HalHandle createHandle(int16_t index, HalHandleEnum handleType);
}
