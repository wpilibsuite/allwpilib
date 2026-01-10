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
class CallbackStore {
 public:
  void create(JNIEnv* env, jobject obj);
  void performCallback(const char* name, const HAL_Value* value);
  void free(JNIEnv* env);
  void setCallbackId(int32_t id) { callbackId = id; }
  int32_t getCallbackId() { return callbackId; }

 private:
  wpi::util::java::JGlobal<jobject> m_call;
  int32_t callbackId;
};

void InitializeStore();

using RegisterCallbackFunc = int32_t (*)(int32_t index,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);
using FreeCallbackFunc = void (*)(int32_t index, int32_t uid);
using RegisterChannelCallbackFunc = int32_t (*)(int32_t index, int32_t channel,
                                                HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify);
using FreeChannelCallbackFunc = void (*)(int32_t index, int32_t channel,
                                         int32_t uid);
using RegisterCallbackNoIndexFunc = int32_t (*)(HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify);
using FreeCallbackNoIndexFunc = void (*)(int32_t uid);

SIM_JniHandle AllocateCallback(JNIEnv* env, jint index, jobject callback,
                               jboolean initialNotify,
                               RegisterCallbackFunc createCallback);
SIM_JniHandle AllocateChannelCallback(
    JNIEnv* env, jint index, jint channel, jobject callback,
    jboolean initialNotify, RegisterChannelCallbackFunc createCallback);
SIM_JniHandle AllocateCallbackNoIndex(
    JNIEnv* env, jobject callback, jboolean initialNotify,
    RegisterCallbackNoIndexFunc createCallback);
void FreeCallback(JNIEnv* env, SIM_JniHandle handle, jint index,
                  FreeCallbackFunc freeCallback);
void FreeChannelCallback(JNIEnv* env, SIM_JniHandle handle, jint index,
                         jint channel, FreeChannelCallbackFunc freeCallback);
void FreeCallbackNoIndex(JNIEnv* env, SIM_JniHandle handle,
                         FreeCallbackNoIndexFunc freeCallback);
}  // namespace wpi::hal::sim
