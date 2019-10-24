#pragma once

#include "hal/Types.h"
#include "hal/AnalogTrigger.h"

#ifdef __cplusplus
extern "C" {
#endif

HAL_DutyCycleHandle HAL_InitializeDutyCycle(HAL_Handle digitalSourceHandle, HAL_AnalogTriggerType triggerType, int32_t* status);
void HAL_FreeDutyCycle(HAL_DutyCycleHandle dutyCycleHandle);

int32_t HAL_ReadDutyCycleFrequency(HAL_DutyCycleHandle dutyCycleHandle, int32_t* status);
int64_t HAL_ReadDutyCycleOutputRaw(HAL_DutyCycleHandle dutyCycleHandle, int32_t* status);
double HAL_ReadDutyCycleOutputPercentage(HAL_DutyCycleHandle dutyCycleHandle, int32_t* status);


#ifdef __cplusplus
}
#endif