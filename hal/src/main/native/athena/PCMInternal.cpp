// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PCMInternal.h"

#include "HALInitializer.h"
#include "hal/Errors.h"
#include "hal/Solenoid.h"

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
  hal::init::CheckInit();
  if (!HAL_CheckSolenoidModule(module)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return;
  }
  if (!PCM_modules[module]) {
    PCM_modules[module] = std::make_unique<PCM>(module);
  }
}

}  // namespace hal
