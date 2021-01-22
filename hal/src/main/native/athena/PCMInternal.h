// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>

#include "PortsInternal.h"
#include "ctre/PCM.h"
#include "hal/Errors.h"
#include "hal/Solenoid.h"

namespace hal {

extern std::unique_ptr<PCM> PCM_modules[kNumPCMModules];

static inline bool checkPCMInit(int32_t module, int32_t* status) {
  if (!HAL_CheckSolenoidModule(module)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return false;
  }
  if (!PCM_modules[module]) {
    *status = INCOMPATIBLE_STATE;
    return false;
  }
  return true;
}

void initializePCM(int32_t module, int32_t* status);

}  // namespace hal
