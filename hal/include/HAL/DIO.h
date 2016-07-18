/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Types.h"

#ifdef __cplusplus
extern "C" {
#endif
HAL_DigitalHandle HAL_InitializeDIOPort(HAL_PortHandle port_handle,
                                        HAL_Bool input, int32_t* status);
void HAL_FreeDIOPort(HAL_DigitalHandle dio_port_handle);

HAL_DigitalPWMHandle HAL_AllocateDigitalPWM(int32_t* status);
void HAL_FreeDigitalPWM(HAL_DigitalPWMHandle pwmGenerator, int32_t* status);
void HAL_SetDigitalPWMRate(double rate, int32_t* status);
void HAL_SetDigitalPWMDutyCycle(HAL_DigitalPWMHandle pwmGenerator,
                                double dutyCycle, int32_t* status);
void HAL_SetDigitalPWMOutputChannel(HAL_DigitalPWMHandle pwmGenerator,
                                    int32_t pin, int32_t* status);
void HAL_SetDIO(HAL_DigitalHandle dio_port_handle, HAL_Bool value,
                int32_t* status);
HAL_Bool HAL_GetDIO(HAL_DigitalHandle dio_port_handle, int32_t* status);
HAL_Bool HAL_GetDIODirection(HAL_DigitalHandle dio_port_handle,
                             int32_t* status);
void HAL_Pulse(HAL_DigitalHandle dio_port_handle, double pulseLength,
               int32_t* status);
HAL_Bool HAL_IsPulsing(HAL_DigitalHandle dio_port_handle, int32_t* status);
HAL_Bool HAL_IsAnyPulsing(int32_t* status);

void HAL_SetFilterSelect(HAL_DigitalHandle dio_port_handle,
                         int32_t filter_index, int32_t* status);
int32_t HAL_GetFilterSelect(HAL_DigitalHandle dio_port_handle, int32_t* status);
void HAL_SetFilterPeriod(int32_t filter_index, int64_t value, int32_t* status);
int64_t HAL_GetFilterPeriod(int32_t filter_index, int32_t* status);
#ifdef __cplusplus
}
#endif
