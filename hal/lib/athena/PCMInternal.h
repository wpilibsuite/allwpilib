/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "ctre/PCM.h"

namespace hal {
constexpr int NUM_MODULE_NUMBERS = 63;
constexpr int NUM_SOLENOID_PINS = 8;

extern PCM* PCM_modules[NUM_MODULE_NUMBERS];

void initializePCM(int module);
}
