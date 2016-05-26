/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
void* initializeNotifier(void (*process)(uint64_t, void*), void* param,
                         int32_t* status);
void cleanNotifier(void* notifier_pointer, int32_t* status);
void* getNotifierParam(void* notifier_pointer, int32_t* status);
void updateNotifierAlarm(void* notifier_pointer, uint64_t triggerTime,
                         int32_t* status);
void stopNotifierAlarm(void* notifier_pointer, int32_t* status);
}
