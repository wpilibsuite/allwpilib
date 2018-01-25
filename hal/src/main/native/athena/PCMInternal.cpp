/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PCMInternal.h"

#include "HAL/Errors.h"
#include "HAL/Solenoid.h"
#include "HAL/cpp/make_unique.h"
#include "PortsInternal.h"

namespace hal {

std::unique_ptr<PCM> PCM_modules[kNumPCMModules];

namespace init {
void InitializePCMInternal() {
  for (int i = 0; i < kNumPCMModules; i++) {
    PCM_modules[i] = nullptr;
  }
}
}  // namespace init

void initializePCM(int32_t module, int32_t* status) {
  if (!HAL_CheckSolenoidModule(module)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return;
  }
  if (!PCM_modules[module]) {
    PCM_modules[module] = std::make_unique<PCM>(module);
  }
}

}  // namespace hal
