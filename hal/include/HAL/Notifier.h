/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Handles.h"

typedef HalHandle HalNotifierHandle;

extern "C" {
HalNotifierHandle initializeNotifier(void (*process)(uint64_t, void*),
                                     void* param, int32_t* status);
void cleanNotifier(HalNotifierHandle notifier_handle, int32_t* status);
void* getNotifierParam(HalNotifierHandle notifier_handle, int32_t* status);
void updateNotifierAlarm(HalNotifierHandle notifier_handle,
                         uint64_t triggerTime, int32_t* status);
void stopNotifierAlarm(HalNotifierHandle notifier_handle, int32_t* status);
}
