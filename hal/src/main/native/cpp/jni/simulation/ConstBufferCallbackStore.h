// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <jni.h>

#include "SimulatorJNI.h"
#include "wpi/hal/Types.h"
#include "wpi/hal/Value.h"
#include "wpi/hal/handles/UnlimitedHandleResource.hpp"
#include "wpi/hal/simulation/NotifyListener.h"
#include "wpi/util/jni_util.hpp"

namespace wpi::hal::sim {
class ConstBufferCallbackStore {
 public:
  void create(JNIEnv* env, jobject obj);
  void performCallback(const char* name, const uint8_t* buffer,
                       uint32_t length);
  void free(JNIEnv* env);
  void setCallbackId(int32_t id) { callbackId = id; }
  int32_t getCallbackId() { return callbackId; }

 private:
  wpi::util::java::JGlobal<jobject> m_call;
  int32_t callbackId;
};

void InitializeConstBufferStore();

using RegisterConstBufferCallbackFunc =
    int32_t (*)(int32_t index, HAL_ConstBufferCallback callback, void* param);
using FreeConstBufferCallbackFunc = void (*)(int32_t index, int32_t uid);

SIM_JniHandle AllocateConstBufferCallback(
    JNIEnv* env, jint index, jobject callback,
    RegisterConstBufferCallbackFunc createCallback);
void FreeConstBufferCallback(JNIEnv* env, SIM_JniHandle handle, jint index,
                             FreeConstBufferCallbackFunc freeCallback);
}  // namespace wpi::hal::sim
