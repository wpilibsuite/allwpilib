/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#define HAL_kInvalidHandle 0

typedef int32_t HAL_Handle;

typedef HAL_Handle HAL_PortHandle;

typedef HAL_Handle HAL_AnalogInputHandle;

typedef HAL_Handle HAL_AnalogOutputHandle;

typedef HAL_Handle HAL_AnalogTriggerHandle;

typedef HAL_Handle HAL_CompressorHandle;

typedef HAL_Handle HAL_CounterHandle;

typedef HAL_Handle HAL_DigitalHandle;

typedef HAL_Handle HAL_DigitalPWMHandle;

typedef HAL_Handle HAL_EncoderHandle;

typedef HAL_Handle HAL_FPGAEncoderHandle;

typedef HAL_Handle HAL_GyroHandle;

typedef HAL_Handle HAL_InterruptHandle;

typedef HAL_Handle HAL_NotifierHandle;

typedef HAL_Handle HAL_RelayHandle;

typedef HAL_Handle HAL_SolenoidHandle;

typedef int32_t HAL_Bool;
