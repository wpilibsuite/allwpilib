/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "HAL/Types.h"

extern "C" {
HAL_NotifierHandle HAL_InitializeNotifierNonThreadedUnsafe(
    HAL_NotifierProcessFunction process, void* param, int32_t* status);
}
