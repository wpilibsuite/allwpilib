/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/MockHooks.h"

namespace frc {
namespace sim {

void WaitForProgramStart() { HALSIM_WaitForProgramStart(); }

void SetProgramStarted() { HALSIM_SetProgramStarted(); }

bool GetProgramStarted() { return HALSIM_GetProgramStarted(); }

void RestartTiming() { HALSIM_RestartTiming(); }

}  // namespace sim
}  // namespace frc
