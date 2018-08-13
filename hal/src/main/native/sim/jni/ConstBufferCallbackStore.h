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
class ConstBufferCallbackStore {
 public:
  void create(JNIEnv* env, jobject obj);
  void performCallback(const char* name, const uint8_t* buffer,
                       uint32_t length);
  void free(JNIEnv* env);
  void setCallbackId(int32_t id) { callbackId = id; }
  int32_t getCallbackId() { return callbackId; }

 private:
  wpi::java::JGlobal<jobject> m_call;
  int32_t callbackId;
};

void InitializeConstBufferStore();

typedef int32_t (*RegisterConstBufferCallbackFunc)(
    int32_t index, HAL_ConstBufferCallback callback, void* param);
typedef void (*FreeConstBufferCallbackFunc)(int32_t index, int32_t uid);

SIM_JniHandle AllocateConstBufferCallback(
    JNIEnv* env, jint index, jobject callback,
    RegisterConstBufferCallbackFunc createCallback);
void FreeConstBufferCallback(JNIEnv* env, SIM_JniHandle handle, jint index,
                             FreeConstBufferCallbackFunc freeCallback);
}  // namespace sim
