// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CallbackStore.h"

#include <jni.h>

#include <cstdio>
#include <memory>

#include <wpi/jni_util.h>

#include "SimulatorJNI.h"
#include "hal/Types.h"
#include "hal/Value.h"
#include "hal/handles/UnlimitedHandleResource.h"

using namespace hal;
using namespace hal::sim;
using namespace wpi::java;

static hal::UnlimitedHandleResource<SIM_JniHandle, CallbackStore,
                                    hal::HAL_HandleEnum::SimulationJni>*
    callbackHandles;

namespace hal::sim {
void InitializeStore() {
  static hal::UnlimitedHandleResource<SIM_JniHandle, CallbackStore,
                                      hal::HAL_HandleEnum::SimulationJni>
      cb;
  callbackHandles = &cb;
}
}  // namespace hal::sim

void CallbackStore::create(JNIEnv* env, jobject obj) {
  m_call = JGlobal<jobject>(env, obj);
}

void CallbackStore::performCallback(const char* name, const HAL_Value* value) {
  JNIEnv* env;
  JavaVM* vm = sim::GetJVM();
  bool didAttachThread = false;
  int tryGetEnv = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
  if (tryGetEnv == JNI_EDETACHED) {
    // Thread not attached
    didAttachThread = true;
    if (vm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != 0) {
      // Failed to attach, log and return
      std::puts("Failed to attach");
      std::fflush(stdout);
      return;
    }
  } else if (tryGetEnv == JNI_EVERSION) {
    std::puts("Invalid JVM Version requested");
    std::fflush(stdout);
  }

  int64_t longValue = 0;

  switch (value->type) {
    case HAL_BOOLEAN:
      longValue = value->data.v_boolean;
      break;
    case HAL_ENUM:
      longValue = value->data.v_enum;
      break;
    case HAL_INT:
      longValue = value->data.v_int;
      break;
    case HAL_LONG:
      longValue = value->data.v_long;
      break;
    case HAL_DOUBLE:
    case HAL_UNASSIGNED:
      break;
  }

  env->CallVoidMethod(m_call, sim::GetNotifyCallback(), MakeJString(env, name),
                      static_cast<jint>(value->type),
                      static_cast<jlong>(longValue),
                      static_cast<jdouble>(value->data.v_double));

  if (env->ExceptionCheck()) {
    env->ExceptionDescribe();
  }

  if (didAttachThread) {
    vm->DetachCurrentThread();
  }
}

void CallbackStore::free(JNIEnv* env) {
  m_call.free(env);
}

SIM_JniHandle sim::AllocateCallback(JNIEnv* env, jint index, jobject callback,
                                    jboolean initialNotify,
                                    RegisterCallbackFunc createCallback) {
  auto callbackStore = std::make_shared<CallbackStore>();

  auto handle = callbackHandles->Allocate(callbackStore);

  if (handle == HAL_kInvalidHandle) {
    return -1;
  }

  uintptr_t handleAsPtr = static_cast<uintptr_t>(handle);
  void* handleAsVoidPtr = reinterpret_cast<void*>(handleAsPtr);

  callbackStore->create(env, callback);

  auto callbackFunc = [](const char* name, void* param,
                         const HAL_Value* value) {
    uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
    SIM_JniHandle handle = static_cast<SIM_JniHandle>(handleTmp);
    auto data = callbackHandles->Get(handle);
    if (!data) {
      return;
    }

    data->performCallback(name, value);
  };

  auto id = createCallback(index, callbackFunc, handleAsVoidPtr, initialNotify);

  callbackStore->setCallbackId(id);

  return handle;
}

void sim::FreeCallback(JNIEnv* env, SIM_JniHandle handle, jint index,
                       FreeCallbackFunc freeCallback) {
  auto callback = callbackHandles->Free(handle);
  freeCallback(index, callback->getCallbackId());
  callback->free(env);
}

SIM_JniHandle sim::AllocateChannelCallback(
    JNIEnv* env, jint index, jint channel, jobject callback,
    jboolean initialNotify, RegisterChannelCallbackFunc createCallback) {
  auto callbackStore = std::make_shared<CallbackStore>();

  auto handle = callbackHandles->Allocate(callbackStore);

  if (handle == HAL_kInvalidHandle) {
    return -1;
  }

  uintptr_t handleAsPtr = static_cast<uintptr_t>(handle);
  void* handleAsVoidPtr = reinterpret_cast<void*>(handleAsPtr);

  callbackStore->create(env, callback);

  auto callbackFunc = [](const char* name, void* param,
                         const HAL_Value* value) {
    uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
    SIM_JniHandle handle = static_cast<SIM_JniHandle>(handleTmp);
    auto data = callbackHandles->Get(handle);
    if (!data) {
      return;
    }

    data->performCallback(name, value);
  };

  auto id = createCallback(index, channel, callbackFunc, handleAsVoidPtr,
                           initialNotify);

  callbackStore->setCallbackId(id);

  return handle;
}

void sim::FreeChannelCallback(JNIEnv* env, SIM_JniHandle handle, jint index,
                              jint channel,
                              FreeChannelCallbackFunc freeCallback) {
  auto callback = callbackHandles->Free(handle);
  freeCallback(index, channel, callback->getCallbackId());
  callback->free(env);
}

SIM_JniHandle sim::AllocateCallbackNoIndex(
    JNIEnv* env, jobject callback, jboolean initialNotify,
    RegisterCallbackNoIndexFunc createCallback) {
  auto callbackStore = std::make_shared<CallbackStore>();

  auto handle = callbackHandles->Allocate(callbackStore);

  if (handle == HAL_kInvalidHandle) {
    return -1;
  }

  uintptr_t handleAsPtr = static_cast<uintptr_t>(handle);
  void* handleAsVoidPtr = reinterpret_cast<void*>(handleAsPtr);

  callbackStore->create(env, callback);

  auto callbackFunc = [](const char* name, void* param,
                         const HAL_Value* value) {
    uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
    SIM_JniHandle handle = static_cast<SIM_JniHandle>(handleTmp);
    auto data = callbackHandles->Get(handle);
    if (!data) {
      return;
    }

    data->performCallback(name, value);
  };

  auto id = createCallback(callbackFunc, handleAsVoidPtr, initialNotify);

  callbackStore->setCallbackId(id);

  return handle;
}

void sim::FreeCallbackNoIndex(JNIEnv* env, SIM_JniHandle handle,
                              FreeCallbackNoIndexFunc freeCallback) {
  auto callback = callbackHandles->Free(handle);
  freeCallback(callback->getCallbackId());
  callback->free(env);
}
