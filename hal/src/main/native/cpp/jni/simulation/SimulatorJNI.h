// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>

#include "hal/Types.h"
#include "jni.h"

using SIM_JniHandle = HAL_Handle;  // NOLINT

struct HALSIM_OpModeOption;

namespace hal::sim {
JavaVM* GetJVM();

jmethodID GetNotifyCallback();
jmethodID GetBufferCallback();
jmethodID GetConstBufferCallback();
jmethodID GetBiConsumerCallback();
jobject CreateOpModeOption(JNIEnv* env, const HALSIM_OpModeOption& option);
jobjectArray CreateOpModeOptionArray(JNIEnv* env, std::span<const HALSIM_OpModeOption> options);
}  // namespace hal::sim
