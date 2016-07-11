/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Handles.h"

extern "C" {
HAL_DigitalHandle HAL_InitializeDIOPort(HAL_PortHandle port_handle,
                                        uint8_t input, int32_t* status);
void HAL_FreeDIOPort(HAL_DigitalHandle dio_port_handle);

HAL_DigitalPWMHandle HAL_AllocateDigitalPWM(int32_t* status);
void HAL_FreeDigitalPWM(HAL_DigitalPWMHandle pwmGenerator, int32_t* status);
void HAL_SetDigitalPWMRate(double rate, int32_t* status);
void HAL_SetDigitalPWMDutyCycle(HAL_DigitalPWMHandle pwmGenerator,
                                double dutyCycle, int32_t* status);
void HAL_SetDigitalPWMOutputChannel(HAL_DigitalPWMHandle pwmGenerator,
                                    uint32_t pin, int32_t* status);

void HAL_SetDIO(HAL_DigitalHandle dio_port_handle, int16_t value,
                int32_t* status);
bool HAL_GetDIO(HAL_DigitalHandle dio_port_handle, int32_t* status);
bool HAL_GetDIODirection(HAL_DigitalHandle dio_port_handle, int32_t* status);
void HAL_Pulse(HAL_DigitalHandle dio_port_handle, double pulseLength,
               int32_t* status);
bool HAL_IsPulsing(HAL_DigitalHandle dio_port_handle, int32_t* status);
bool HAL_IsAnyPulsing(int32_t* status);

void HAL_SetFilterSelect(HAL_DigitalHandle dio_port_handle, int filter_index,
                         int32_t* status);
int HAL_GetFilterSelect(HAL_DigitalHandle dio_port_handle, int32_t* status);

void HAL_SetFilterPeriod(int filter_index, uint32_t value, int32_t* status);
uint32_t HAL_GetFilterPeriod(int filter_index, int32_t* status);
}
