/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <jni.h>

#include <wpi/jni_util.h>

#include "SimulatorJNI.h"
#include "hal/Types.h"
#include "hal/handles/UnlimitedHandleResource.h"
#include "mockdata/HAL_Value.h"
#include "mockdata/NotifyListener.h"

namespace sim {
class CallbackStore {
 public:
  void create(JNIEnv* env, jobject obj);
  void performCallback(const char* name, const HAL_Value* value);
  void free(JNIEnv* env);
  void setCallbackId(int32_t id) { callbackId = id; }
  int32_t getCallbackId() { return callbackId; }

 private:
  wpi::java::JGlobal<jobject> m_call;
  int32_t callbackId;
};

void InitializeStore();

typedef int32_t (*RegisterCallbackFunc)(int32_t index,
                                        HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);
typedef void (*FreeCallbackFunc)(int32_t index, int32_t uid);
typedef int32_t (*RegisterChannelCallbackFunc)(int32_t index, int32_t channel,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify);
typedef void (*FreeChannelCallbackFunc)(int32_t index, int32_t channel,
                                        int32_t uid);
typedef int32_t (*RegisterCallbackNoIndexFunc)(HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify);
typedef void (*FreeCallbackNoIndexFunc)(int32_t uid);

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
}  // namespace sim
