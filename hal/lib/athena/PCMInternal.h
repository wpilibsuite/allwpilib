/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Ports.h"
#include "PortsInternal.h"
#include "ctre/PCM.h"

namespace hal {
extern PCM* PCM_modules[kNumPCMModules];

void initializePCM(int module);
}
