// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "OpModeOptionsCallbackStore.h"

#include <jni.h>

#include <cstdio>
#include <memory>

#include "../HALUtil.h"
#include "SimulatorJNI.h"
#include "wpi/hal/Types.h"
#include "wpi/hal/handles/UnlimitedHandleResource.hpp"
#include "wpi/util/jni_util.hpp"

using namespace wpi::hal;
using namespace wpi::hal::sim;
using namespace wpi::util::java;

static UnlimitedHandleResource<SIM_JniHandle, OpModeOptionsCallbackStore,
                               HAL_HandleEnum::SimulationJni>* callbackHandles;

namespace wpi::hal::sim {
void InitializeOpModeOptionsStore() {
  static UnlimitedHandleResource<SIM_JniHandle, OpModeOptionsCallbackStore,
                                 HAL_HandleEnum::SimulationJni>
      cb;
  callbackHandles = &cb;
}
}  // namespace wpi::hal::sim

void OpModeOptionsCallbackStore::create(JNIEnv* env, jobject obj) {
  m_call = JGlobal<jobject>(env, obj);
}

void OpModeOptionsCallbackStore::performCallback(
    const char* name, const HAL_OpModeOption* opmodes, int32_t count) {
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

  JLocal<jobjectArray> toCallbackArr{
      env, CreateOpModeOptionArray(env, {opmodes, opmodes + count})};

  env->CallVoidMethod(m_call, sim::GetBiConsumerCallback(),
                      MakeJString(env, name), toCallbackArr.obj());

  if (env->ExceptionCheck()) {
    env->ExceptionDescribe();
  }

  if (didAttachThread) {
    vm->DetachCurrentThread();
  }
}

void OpModeOptionsCallbackStore::free(JNIEnv* env) {
  m_call.free(env);
}

SIM_JniHandle sim::AllocateOpModeOptionsCallback(
    JNIEnv* env, jobject callback, jboolean initialNotify,
    RegisterOpModeOptionsCallbackFunc createCallback) {
  auto callbackStore = std::make_shared<OpModeOptionsCallbackStore>();

  auto handle = callbackHandles->Allocate(callbackStore);

  if (handle == HAL_kInvalidHandle) {
    return -1;
  }

  uintptr_t handleAsPtr = static_cast<uintptr_t>(handle);
  void* handleAsVoidPtr = reinterpret_cast<void*>(handleAsPtr);

  callbackStore->create(env, callback);

  auto callbackFunc = [](const char* name, void* param,
                         const HAL_OpModeOption* opmodes, int32_t count) {
    uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
    SIM_JniHandle handle = static_cast<SIM_JniHandle>(handleTmp);
    auto data = callbackHandles->Get(handle);
    if (!data) {
      return;
    }

    data->performCallback(name, opmodes, count);
  };

  auto id = createCallback(callbackFunc, handleAsVoidPtr, initialNotify);

  callbackStore->setCallbackId(id);

  return handle;
}

void sim::FreeOpModeOptionsCallback(
    JNIEnv* env, SIM_JniHandle handle,
    FreeOpModeOptionsCallbackFunc freeCallback) {
  auto callback = callbackHandles->Free(handle);
  if (callback == nullptr) {
    return;
  }
  freeCallback(callback->getCallbackId());
  callback->free(env);
}
