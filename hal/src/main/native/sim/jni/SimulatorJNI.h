/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/jni_util.h>

#include "hal/Types.h"
#include "jni.h"
#include "mockdata/HAL_Value.h"

typedef HAL_Handle SIM_JniHandle;

namespace sim {
JavaVM* GetJVM();

jmethodID GetNotifyCallback();
jmethodID GetBufferCallback();
jmethodID GetConstBufferCallback();
jmethodID GetSpiReadAutoReceiveBufferCallback();
}  // namespace sim
