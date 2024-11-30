// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/NotifierData.h"

extern "C" {

uint64_t HALSIM_GetNextNotifierTimeout(void) {
  return 0;
}

int32_t HALSIM_GetNumNotifiers(void) {
  return 0;
}

int32_t HALSIM_GetNotifierInfo(struct HALSIM_NotifierInfo* arr, int32_t size) {
  return 0;
}

}  // extern "C"
