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
class BufferCallbackStore {
 public:
  void create(JNIEnv* env, jobject obj);
  void performCallback(const char* name, uint8_t* buffer, uint32_t length);
  void free(JNIEnv* env);
  void setCallbackId(int32_t id) { callbackId = id; }
  int32_t getCallbackId() { return callbackId; }

 private:
  wpi::java::JGlobal<jobject> m_call;
  int32_t callbackId;
};

void InitializeBufferStore();

typedef int32_t (*RegisterBufferCallbackFunc)(int32_t index,
                                              HAL_BufferCallback callback,
                                              void* param);
typedef void (*FreeBufferCallbackFunc)(int32_t index, int32_t uid);

SIM_JniHandle AllocateBufferCallback(JNIEnv* env, jint index, jobject callback,
                                     RegisterBufferCallbackFunc createCallback);
void FreeBufferCallback(JNIEnv* env, SIM_JniHandle handle, jint index,
                        FreeBufferCallbackFunc freeCallback);
}  // namespace sim
