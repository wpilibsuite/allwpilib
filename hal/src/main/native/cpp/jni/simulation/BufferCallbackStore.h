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
class BufferCallbackStore {
 public:
  void create(JNIEnv* env, jobject obj);
  void performCallback(const char* name, uint8_t* buffer, uint32_t length);
  void free(JNIEnv* env);
  void setCallbackId(int32_t id) { callbackId = id; }
  int32_t getCallbackId() { return callbackId; }

 private:
  wpi::util::java::JGlobal<jobject> m_call;
  int32_t callbackId;
};

void InitializeBufferStore();

using RegisterBufferCallbackFunc = int32_t (*)(int32_t index,
                                               HAL_BufferCallback callback,
                                               void* param);
using FreeBufferCallbackFunc = void (*)(int32_t index, int32_t uid);

SIM_JniHandle AllocateBufferCallback(JNIEnv* env, jint index, jobject callback,
                                     RegisterBufferCallbackFunc createCallback);
void FreeBufferCallback(JNIEnv* env, SIM_JniHandle handle, jint index,
                        FreeBufferCallbackFunc freeCallback);
}  // namespace wpi::hal::sim
