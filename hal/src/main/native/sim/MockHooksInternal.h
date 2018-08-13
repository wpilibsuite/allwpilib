/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "mockdata/MockHooks.h"

namespace hal {
void RestartTiming();

int64_t GetFPGATime();

double GetFPGATimestamp();

void SetProgramStarted();
}  // namespace hal
