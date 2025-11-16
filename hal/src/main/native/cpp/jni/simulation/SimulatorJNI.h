// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "jni.h"
#include "wpi/hal/Types.h"

using SIM_JniHandle = HAL_Handle;  // NOLINT

namespace wpi::hal::sim {
JavaVM* GetJVM();

jmethodID GetNotifyCallback();
jmethodID GetBufferCallback();
jmethodID GetConstBufferCallback();
}  // namespace wpi::hal::sim
