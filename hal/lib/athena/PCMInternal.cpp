/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PCMInternal.h"

#include "HAL/Errors.h"
#include "HAL/Solenoid.h"
#include "PortsInternal.h"

namespace hal {
PCM* PCM_modules[kNumPCMModules] = {nullptr};

void initializePCM(int32_t module, int32_t* status) {
  if (!HAL_CheckSolenoidModule(module)) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }
  if (!PCM_modules[module]) {
    PCM_modules[module] = new PCM(module);
  }
}
}  // namespace hal
