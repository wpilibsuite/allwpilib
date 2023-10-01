// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/NetworkUsage.h"

#include <wpi/MemoryBuffer.h>
#include <wpi/StringExtras.h>

#include "hal/Errors.h"
#include "hal/simulation/RoboRioData.h"

using namespace hal;

extern "C" {

void HAL_GetNetworkStatus(int32_t* rxBytes, int32_t* txBytes, int32_t* status) {
  *rxBytes = HALSIM_GetRoboRioNetworkRxBytes();
  *txBytes = HALSIM_GetRoboRioNetworkTxBytes();
}

}  // extern "C"
