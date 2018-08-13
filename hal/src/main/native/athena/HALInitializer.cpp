/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALInitializer.h"

#include "hal/HAL.h"

namespace hal {
namespace init {
std::atomic_bool HAL_IsInitialized{false};
void RunInitialize() { HAL_Initialize(500, 0); }
}  // namespace init
}  // namespace hal
