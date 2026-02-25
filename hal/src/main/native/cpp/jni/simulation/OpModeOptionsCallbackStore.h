// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <jni.h>

#include "SimulatorJNI.h"
#include "wpi/hal/Types.h"
#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/util/jni_util.hpp"

namespace wpi::hal::sim {
class OpModeOptionsCallbackStore {
 public:
  void create(JNIEnv* env, jobject obj);
  void performCallback(const char* name, const HAL_OpModeOption* opmodes,
                       int32_t count);
  void free(JNIEnv* env);
  void setCallbackId(int32_t id) { callbackId = id; }
  int32_t getCallbackId() { return callbackId; }

 private:
  wpi::util::java::JGlobal<jobject> m_call;
  int32_t callbackId;
};

void InitializeOpModeOptionsStore();

using RegisterOpModeOptionsCallbackFunc = int32_t (*)(
    HAL_OpModeOptionsCallback callback, void* param, HAL_Bool initialNotify);
using FreeOpModeOptionsCallbackFunc = void (*)(int32_t uid);

SIM_JniHandle AllocateOpModeOptionsCallback(
    JNIEnv* env, jobject callback, jboolean initialNotify,
    RegisterOpModeOptionsCallbackFunc createCallback);
void FreeOpModeOptionsCallback(JNIEnv* env, SIM_JniHandle handle,
                               FreeOpModeOptionsCallbackFunc freeCallback);
}  // namespace wpi::hal::sim
