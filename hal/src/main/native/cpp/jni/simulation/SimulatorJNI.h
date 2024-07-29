// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"
#include "jni.h"

using SIM_JniHandle = HAL_Handle;  // NOLINT

namespace hal::sim {
JavaVM* GetJVM();

jmethodID GetNotifyCallback();
jmethodID GetBufferCallback();
jmethodID GetConstBufferCallback();
jmethodID GetSpiReadAutoReceiveBufferCallback();
}  // namespace hal::sim
