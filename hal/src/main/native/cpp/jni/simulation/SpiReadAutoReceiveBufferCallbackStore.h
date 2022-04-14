// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <jni.h>

#include <wpi/jni_util.h>

#include "SimulatorJNI.h"
#include "hal/Types.h"
#include "hal/Value.h"
#include "hal/handles/UnlimitedHandleResource.h"
#include "hal/simulation/NotifyListener.h"
#include "hal/simulation/SPIData.h"

namespace hal::sim {
class SpiReadAutoReceiveBufferCallbackStore {
 public:
  void create(JNIEnv* env, jobject obj);
  int32_t performCallback(const char* name, uint32_t* buffer,
                          int32_t numToRead);
  void free(JNIEnv* env);
  void setCallbackId(int32_t id) { callbackId = id; }
  int32_t getCallbackId() { return callbackId; }

 private:
  wpi::java::JGlobal<jobject> m_call;
  int32_t callbackId;
};

void InitializeSpiBufferStore();

using RegisterSpiBufferCallbackFunc = int32_t (*)(
    int32_t index, HAL_SpiReadAutoReceiveBufferCallback callback, void* param);
using FreeSpiBufferCallbackFunc = void (*)(int32_t index, int32_t uid);

SIM_JniHandle AllocateSpiBufferCallback(
    JNIEnv* env, jint index, jobject callback,
    RegisterSpiBufferCallbackFunc createCallback);
void FreeSpiBufferCallback(JNIEnv* env, SIM_JniHandle handle, jint index,
                           FreeSpiBufferCallbackFunc freeCallback);
}  // namespace hal::sim
