/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Constants.h"

#include "ConstantsInternal.h"

using namespace hal;

extern "C" {
int32_t HAL_GetSystemClockTicksPerMicrosecond(void) {
  return kSystemClockTicksPerMicrosecond;
}
}
