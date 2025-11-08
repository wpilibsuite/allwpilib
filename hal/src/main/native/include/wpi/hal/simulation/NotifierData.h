// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct HALSIM_NotifierInfo {
  HAL_NotifierHandle handle;
  char name[64];
  uint64_t timeout;
  HAL_Bool waitTimeValid;
};

uint64_t HALSIM_GetNextNotifierTimeout(void);

int32_t HALSIM_GetNumNotifiers(void);

/**
 * Gets detailed information about each notifier.
 *
 * @param arr array of information to be filled
 * @param size size of arr
 * @return Number of notifiers; note: may be larger than passed-in size
 */
int32_t HALSIM_GetNotifierInfo(struct HALSIM_NotifierInfo* arr, int32_t size);

#ifdef __cplusplus
}  // extern "C"
#endif
