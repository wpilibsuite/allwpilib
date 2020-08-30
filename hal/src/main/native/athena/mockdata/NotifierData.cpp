/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/NotifierData.h"

extern "C" {

uint64_t HALSIM_GetNextNotifierTimeout(void) { return 0; }

int32_t HALSIM_GetNumNotifiers(void) { return 0; }

int32_t HALSIM_GetNotifierInfo(struct HALSIM_NotifierInfo* arr, int32_t size) {
  return 0;
}

}  // extern "C"
