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
  HalHandle handle = handleType;
  handle = handle << 24;
  handle += index;
  return handle;
}
}
