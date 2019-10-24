#pragma once

#include "hal/Types.h"
#include "hal/AnalogTrigger.h"

#ifdef __cplusplus
extern "C" {
#endif

HAL_DutyCycleHandle HAL_InitializeDutyCycle(HAL_Handle digitalSourceHandle, HAL_AnalogTriggerType triggerType, int32_t* status);
void HAL_FreeDutyCycle(HAL_DutyCycleHandle dutyCycleHandle);

int32_t HAL_GetDutyCycleFrequency(HAL_DutyCycleHandle dutyCycleHandle, int32_t* status);
int64_t HAL_GetDutyCycleOutputRaw(HAL_DutyCycleHandle dutyCycleHandle, int32_t* status);
double HAL_GetDutyCycleOutputScaled(HAL_DutyCycleHandle dutyCycleHandle, int32_t* status);


#ifdef __cplusplus
}
#endif