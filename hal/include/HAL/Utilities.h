/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
extern const int32_t HAL_NO_WAIT;
extern const int32_t HAL_WAIT_FOREVER;

void delayTicks(int32_t ticks);
void delayMillis(double ms);
void delaySeconds(double s);
}
