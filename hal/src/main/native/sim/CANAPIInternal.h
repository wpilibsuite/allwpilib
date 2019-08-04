/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "hal/Types.h"

namespace hal {
namespace can {
int32_t GetCANModuleFromHandle(HAL_CANHandle handle, int32_t* status);
}  // namespace can
}  // namespace hal
