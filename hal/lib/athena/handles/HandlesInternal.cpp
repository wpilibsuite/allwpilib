/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HandlesInternal.h"
#include "IndexedHandleResource.h"
#include "LimitedHandleResource.h"
#include "UnlimitedHandleResource.h"

namespace hal {
HalHandle createHandle(int16_t index, HalHandleEnum handleType) {
  if (index < 0) return HAL_HANDLE_NEGATIVE_INDEX;
  uint8_t hType = static_cast<uint8_t>(handleType);
  if (hType == 0 || hType > 127) return HAL_HANDLE_INVALID_TYPE;
  // set last 8 bits, then shift to first 8 bits
  HalHandle handle = hType;
  handle = handle << 24;
  // add index to set last 16 bits
  handle += index;
  return handle;
}
}
