/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Constants.h"

#include "ConstantsInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeConstants() {}
}  // namespace init
}  // namespace hal

extern "C" {
int32_t HAL_GetSystemClockTicksPerMicrosecond(void) {
  return kSystemClockTicksPerMicrosecond;
}
}  // extern "C"
