/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PCMInternal.h"

#include "PortsInternal.h"

namespace hal {
PCM* PCM_modules[kNumPCMModules] = {nullptr};

void initializePCM(int module) {
  if (!PCM_modules[module]) {
    PCM_modules[module] = new PCM(module);
  }
}
}
